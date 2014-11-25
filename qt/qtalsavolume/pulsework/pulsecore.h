/*
 * Copyright (C) 2014 Vitaly Tonkacheyev <thetvg@gmail.com>
 *
 * Big thanks to Clément Démoulins <clement@archivel.fr>
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

#ifndef PULSECORE_H
#define PULSECORE_H

#include "pulsedevice.h"
#include "pulse/pulseaudio.h"
#include <QStringList>
#include <QList>
#include <QPair>
#include <QSharedPointer>

struct ServerInfo {
	QString defaultSourceName;
	QString defaultSinkName;
};
enum state {
	CONNECTING,
	CONNECTED,
	ERROR
};

typedef QList<PulseDevice::Ptr> PulseDevicePtrList;

class PulseCore
{
public:
	PulseCore(const char *clientName);
	~PulseCore();
	typedef QSharedPointer<PulseCore> Ptr;
	state pState;
	const QString defaultSink();
	const QStringList &getCardList() const;
	const QString getDeviceDescription(const QString &name);
	const QString getDeviceNameByIndex(int index);
	int getCurrentDeviceIndex() const;
	int getVolume();
	bool getMute();
	void setVolume(int value);
	void setMute(bool mute);
	void setCurrentDevice(const QString &name);
	void refreshDevices();
	bool available();
private:
	void getSinks();
	void getSources();
	PulseDevice::Ptr getSink(int index);
	PulseDevice::Ptr getSink(const QString &name);
	PulseDevice::Ptr getSource(int index);
	PulseDevice::Ptr getSource(const QString &name);
	PulseDevice::Ptr getDefaultSink();
	PulseDevice::Ptr getDefaultSource();
	PulseDevice::Ptr getDeviceByName(const QString &name);
	PulseDevice::Ptr getDeviceByIndex(int index);
	int getDeviceIndexByName(const QString &name);
	void setVolume_(const PulseDevice::Ptr &device, int value);
	void setMute_(const PulseDevice::Ptr &device, bool mute);
	void iterate(pa_operation* op);
	void onError(const QString &message);
	void updateDevices();
	void clearLists();
private:
	pa_mainloop* mainLoop_;
	pa_mainloop_api* mainLoopApi_;
	pa_context* context_;
	int retval_;
	//PulseDevice::Ptr currentDevice_;
	QString currentDeviceName_;
	int currentDeviceIndex_;
	QStringList sinksDescriptions_;
	QStringList sourcesDescriptions_;
	QStringList deviceNames_;
	QStringList deviceDescriptions_;
	PulseDevicePtrList sinks_;
	PulseDevicePtrList sources_;
	bool isAvailable_;
};

#endif // PULSECORE_H
