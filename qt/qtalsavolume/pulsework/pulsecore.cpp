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
  context_(pa_context_new(mainLoopApi_,clientName))
{
	pState = CONNECTING;
	pa_context_set_state_callback(context_, &state_cb, this);
	pa_context_connect(context_, NULL, PA_CONTEXT_NOFLAGS, NULL);
	while (pState == CONNECTING) {
		pa_mainloop_iterate(mainLoop_, 1, &retval_);
	}
	if (pState == ERROR) {
		onError("Connection Error");
	}
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

QList<PulseDevice> PulseCore::getSinks()
{
	QList<PulseDevice> sinks;
	pa_operation* op = pa_context_get_sink_info_list(context_, &sink_list_cb, &sinks);
	iterate(op);
	pa_operation_unref(op);
	return sinks;
}

QList<PulseDevice> PulseCore::getSources()
{
	QList<PulseDevice> sources;
	pa_operation* op = pa_context_get_source_info_list(context_, &source_list_cb, &sources);
	iterate(op);
	pa_operation_unref(op);
	return sources;
}

PulseDevice PulseCore::getSink(u_int32_t index)
{
	QList<PulseDevice> sinks;
	pa_operation* op = pa_context_get_sink_info_by_index(context_, index, &sink_list_cb, &sinks);
	iterate(op);
	pa_operation_unref(op);
	if (sinks.isEmpty()) {
		onError("The sink doesn't exit");
	}
	return *(sinks.begin());
}

PulseDevice PulseCore::getSink(const QString &name)
{
	QList<PulseDevice> sinks;
	pa_operation* op = pa_context_get_sink_info_by_name(context_, name.toStdString().c_str(), &sink_list_cb, &sinks);
	iterate(op);
	pa_operation_unref(op);
	if (sinks.isEmpty()) {
		onError("The sink doesn't exit");
	}
	return *(sinks.begin());
}

PulseDevice PulseCore::getSource(u_int32_t index)
{
	QList<PulseDevice> sources;
	pa_operation* op = pa_context_get_source_info_by_index(context_, index, &source_list_cb, &sources);
	iterate(op);
	pa_operation_unref(op);
	if (sources.isEmpty()) {
		onError("The source doesn't exit");
	}
	return *(sources.begin());
}

PulseDevice PulseCore::getSource(const QString &name)
{
	QList<PulseDevice> sources;
	pa_operation* op = pa_context_get_source_info_by_name(context_, name.toStdString().c_str(), &source_list_cb, &sources);
	iterate(op);
	pa_operation_unref(op);
	if (sources.isEmpty()) {
		onError("The source doesn't exit");
	}
	return *(sources.begin());
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

QStringList PulseCore::getSinksDescriptions()
{
	QStringList descriptions;
	foreach (PulseDevice device, getSinks()) {
		descriptions << device.description();
	}
	return descriptions;
}

QStringList PulseCore::getSourcesDescriptions()
{
	QStringList descriptions;
	foreach (PulseDevice device, getSources()) {
		descriptions << device.description();
	}
	return descriptions;
}

QString PulseCore::getDeviceDescription(const QString &name)
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
	foreach (PulseDevice device, getSinks()) {
		if (device.name() == name) {
			result = device;
		}
	}
	foreach (PulseDevice device, getSources()) {
		if (device.name() == name) {
			result = device;
		}
	}
	return result;
}

QString PulseCore::getDeviceNameByIndex(int index)
{
	return getDeviceByIndex(index).name();
}

QString PulseCore::defaultSink()
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
	mbox.critical(0, "Error", message);
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

int PulseCore::getVolume()
{
	return currentDevice_->volume_percent();
}

bool PulseCore::getMute()
{
	return currentDevice_->mute();
}

QStringList PulseCore::getCardList()
{
	return QStringList() << getSinksDescriptions() << getSourcesDescriptions();
}

PulseDevice PulseCore::getDeviceByIndex(int index)
{
	PulseDevice device = getDefaultSink();
	QStringList sourcesDesc = getSourcesDescriptions();
	QStringList sinksDesc = getSinksDescriptions();
	int sinksSize = sinksDesc.size();
	int deltaIndex = sinksSize - index;
	if (index < sinksSize) {
		device = getSink(index);
	}
	else if (deltaIndex < sourcesDesc.size()){
		device = getSource(deltaIndex);
	}
	return device;
}

int PulseCore::getCurrentDeviceIndex()
{
	QStringList sinksDesc = getSinksDescriptions();
	int sinksSize = sinksDesc.size();
	int absIndex = currentDevice_->index();
	if (currentDevice_->type() == SINK) {
		return absIndex;
	}
	else {
		return (sinksSize + absIndex);
	}
	return getDefaultSink().index();
}
