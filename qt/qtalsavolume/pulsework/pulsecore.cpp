/*
 * Copyright (C) 2011 Clément Démoulins <clement@archivel.fr>
 * Copyright (C) 2014 Vitaly Tonkacheyev <thetvg@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pulsecore.h"
#include "pulse/pulseaudio.h"

#include <QMessageBox>
#ifdef ISDEBUG
#include <QDebug>
#endif

//Callbacks
void state_cb(pa_context* context, void* raw) {
	PulseCore *state = (PulseCore*) raw;
	switch(pa_context_get_state(context)) {
		case PA_CONTEXT_READY:
			state->pState = CONNECTED;
			break;
		case PA_CONTEXT_FAILED:
			state->pState = ERROR;
			break;
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_TERMINATED:
			break;
	}
}

void sink_list_cb(pa_context *c, const pa_sink_info *i, int eol, void *raw) {
	Q_UNUSED(c);
	if (eol != 0) return;
	QList<PulseDevice>* sinks = (QList<PulseDevice>*) raw;
	PulseDevice s(i);
	sinks->push_back(s);
}

void source_list_cb(pa_context *c, const pa_source_info *i, int eol, void *raw) {
	Q_UNUSED(c);
	if (eol != 0) return;
	QList<PulseDevice>* sources = (QList<PulseDevice>*) raw;
	PulseDevice s(i);
	sources->push_back(s);
}

void server_info_cb(pa_context* c, const pa_server_info* i, void* raw) {
	Q_UNUSED(c);
	ServerInfo* info = (ServerInfo*) raw;
	info->defaultSinkName = QString(i->default_sink_name);
	info->defaultSourceName = QString(i->default_source_name);
}

void success_cb(pa_context* c, int success, void* raw) {
	Q_UNUSED(c);
	Q_UNUSED(success);
	Q_UNUSED(raw);
}
//


PulseCore::PulseCore(const char *clientName)
: mainLoop_(pa_mainloop_new()),
  mainLoopApi_(pa_mainloop_get_api(mainLoop_)),
  context_(pa_context_new(mainLoopApi_,clientName)),
  sinksDescriptions_(QStringList()),
  sourcesDescriptions_(QStringList()),
  deviceNames_(QStringList()),
  deviceDescriptions_(QStringList())
{
	pState = CONNECTING;
	pa_context_set_state_callback(context_, &state_cb, this);
	pa_context_connect(context_, NULL, PA_CONTEXT_NOFLAGS, NULL);
	while (pState == CONNECTING) {
		pa_mainloop_iterate(mainLoop_, 1, &retval_);
	}
	if (pState == ERROR) {
		onError("Error to connect to Pulseaudio main loop");
	}
	updateDevices();
	currentDevice_ = new PulseDevice(getDefaultSink());
}

PulseCore::~PulseCore()
{
	delete currentDevice_;
	if (pState == CONNECTED) {
		pa_context_disconnect(context_);
	}
	pa_mainloop_free(mainLoop_);
}

void PulseCore::iterate(pa_operation *op)
{
	while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
		pa_mainloop_iterate(mainLoop_, 1, &retval_);
	}
}

const QList<PulseDevice> PulseCore::getSinks()
{
	QList<PulseDevice> sinks;
	pa_operation* op = pa_context_get_sink_info_list(context_, &sink_list_cb, &sinks);
	iterate(op);
	pa_operation_unref(op);
	return sinks;
}

const QList<PulseDevice> PulseCore::getSources()
{
	QList<PulseDevice> sources;
	pa_operation* op = pa_context_get_source_info_list(context_, &source_list_cb, &sources);
	iterate(op);
	pa_operation_unref(op);
	return sources;
}

PulseDevice PulseCore::getSink(int index)
{
	if (index >= 0 && (int)index < sinks_.size()) {
		return sinks_.at(index);
	}
	else {
		onError(QString("Pulseaudio sink with id=%1 not exist.\n Default sink will be used").arg(QString::number(index)));
	}
	return getDefaultSink();
}

PulseDevice PulseCore::getSink(const QString &name)
{
	foreach(const PulseDevice &sink, sinks_) {
		if (sink.name() == name) {
			return sink;
		}
	}
	onError(QString("Pulseaudio sink with name=%1 not exist.\n Default sink will be used").arg(name));
	return getDefaultSink();
}

PulseDevice PulseCore::getSource(u_int32_t index)
{
	if (index >= 0 && (int)index < sources_.size()) {
		return sources_.at(index);
	}
	else {
		onError(QString("Pulseaudio source with id=%1 not exist.\n Default source will be used").arg(QString::number(index)));
	}
	return getDefaultSource();
}

PulseDevice PulseCore::getSource(const QString &name)
{
	foreach(const PulseDevice &source, sources_) {
		if (source.name() == name) {
			return source;
		}
	}
	onError(QString("Pulseaudio source with name=%1 not exist.\n Default source will be used").arg(name));
	return getDefaultSource();
}

PulseDevice PulseCore::getDefaultSink()
{
	ServerInfo info;
	pa_operation* op = pa_context_get_server_info(context_, &server_info_cb, &info);
	iterate(op);
	pa_operation_unref(op);
	return getSink(info.defaultSinkName);
}

PulseDevice PulseCore::getDefaultSource()
{
	ServerInfo info;
	pa_operation* op = pa_context_get_server_info(context_, &server_info_cb, &info);
	iterate(op);
	pa_operation_unref(op);
	return getSource(info.defaultSourceName);
}

const QString PulseCore::getDeviceDescription(const QString &name)
{
	QString desc = getDeviceByName(name).description();
	if (desc.isEmpty()) {
		desc = getDefaultSink().description();
	}
	return desc;
}

PulseDevice PulseCore::getDeviceByName(const QString &name)
{
	PulseDevice result = PulseDevice();
	foreach (const PulseDevice &device, sinks_) {
		if (device.name() == name) {
			result = device;
		}
	}
	foreach (const PulseDevice &device, sources_) {
		if (device.name() == name) {
			result = device;
		}
	}
	return result;
}

const QString PulseCore::getDeviceNameByIndex(int index)
{
	return getDeviceByIndex(index).name();
}

const QString PulseCore::defaultSink()
{
	return getDefaultSink().name();
}

void PulseCore::setVolume_(PulseDevice &device, int value)
{
	pa_cvolume* new_cvolume = pa_cvolume_set(&device.volume,
						 device.volume.channels,
						 (pa_volume_t) device.round(qMax(((double)value * PA_VOLUME_NORM) / 100, 0.0))
						 );
	pa_operation* op;
	if (device.type() == SINK) {
		op = pa_context_set_sink_volume_by_index(context_, device.index(), new_cvolume, success_cb, NULL);
	}
	else {
		op = pa_context_set_source_volume_by_index(context_, device.index(), new_cvolume, success_cb, NULL);
	}
	iterate(op);
	pa_operation_unref(op);
}

void PulseCore::setMute_(PulseDevice &device, bool mute)
{
	pa_operation* op;
	if (device.type() == SINK) {
		op = pa_context_set_sink_mute_by_index(context_, device.index(), (int) mute, success_cb, NULL);
	}
	else {
		op = pa_context_set_source_mute_by_index(context_, device.index(), (int) mute, success_cb, NULL);
	}
	iterate(op);
	pa_operation_unref(op);
}

void PulseCore::onError(const QString &message)
{
	QMessageBox mbox;
	mbox.critical(0, "Error in pulsecore.cpp", message);
	mbox.exec();
}

void PulseCore::setCurrentDevice(const QString &name)
{
	currentDevice_ = 0;
	currentDevice_ = new PulseDevice(getDeviceByName(name));
}

void PulseCore::setVolume(int value)
{
	setVolume_((*currentDevice_), value);
}

void PulseCore::setMute(bool mute)
{
	setMute_((*currentDevice_), mute);
}

void PulseCore::updateDevices()
{
	if (!sinks_.isEmpty()) {
		sinks_.clear();
	}
	if (!sources_.isEmpty()) {
		sources_.clear();
	}
	if (!deviceNames_.isEmpty()) {
		deviceNames_.clear();
	}
	if (!deviceDescriptions_.isEmpty()) {
		deviceDescriptions_.clear();
	}
	if (!sinksDescriptions_.isEmpty()) {
		sinksDescriptions_.clear();
	}
	if (!sourcesDescriptions_.isEmpty()) {
		sourcesDescriptions_.clear();
	}
	sinks_ = getSinks();
	sources_ = getSources();
	foreach(const PulseDevice &device, sinks_) {
		deviceNames_ << device.name();
		sinksDescriptions_ << device.description();
	}
	foreach(const PulseDevice &device, sources_) {
		deviceNames_ << device.name();
		sourcesDescriptions_ << device.description();
	}
	deviceDescriptions_ = sinksDescriptions_;
	foreach(const QString &desc, sourcesDescriptions_) {
		deviceDescriptions_ << desc;
	}
}

void PulseCore::refreshDevices()
{
	updateDevices();
}

int PulseCore::getVolume() const
{
	return currentDevice_->volume_percent();
}

bool PulseCore::getMute()
{
	return currentDevice_->mute();
}

const QStringList &PulseCore::getCardList() const
{
	return deviceDescriptions_;
}

PulseDevice PulseCore::getDeviceByIndex(int index)
{
	PulseDevice device = getDefaultSink();
	const int sinksSize = sinksDescriptions_.size();
	const int deltaIndex = abs(sinksSize - index);
	if (index < sinksSize) {
		device = getSink(index);
	}
	else if (deltaIndex < sourcesDescriptions_.size()){
		device = getSource(deltaIndex);
	}
	return device;
}

int PulseCore::getCurrentDeviceIndex() const
{
	return deviceNames_.indexOf(currentDevice_->name());
}
