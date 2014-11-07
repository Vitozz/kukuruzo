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

typedef QSharedPointer<PulseDevice> PulseDevicePtr;
typedef QList<PulseDevicePtr> PulseDevicePtrList;

class PulseCore
{
public:
	PulseCore(const char *clientName);
	~PulseCore();
	state pState;
	const QString defaultSink();
	const QStringList &getCardList() const;
	const QString getDeviceDescription(const QString &name);
	const QString getDeviceNameByIndex(int index);
	int getCurrentDeviceIndex() const;
	int getVolume() const;
	bool getMute();
	void setVolume(int value);
	void setMute(bool mute);
	void setCurrentDevice(const QString &name);
	void refreshDevices();
	bool available();
private:
	void getSinks();
	void getSources();
	PulseDevicePtr getSink(int index);
	PulseDevicePtr getSink(const QString &name);
	PulseDevicePtr getSource(int index);
	PulseDevicePtr getSource(const QString &name);
	PulseDevicePtr getDefaultSink();
	PulseDevicePtr getDefaultSource();
	PulseDevicePtr getDeviceByName(const QString &name);
	PulseDevicePtr getDeviceByIndex(int index);
	void setVolume_(PulseDevicePtr &device, int value);
	void setMute_(PulseDevicePtr &device, bool mute);
	void iterate(pa_operation* op);
	void onError(const QString &message);
	void updateDevices();
private:
	pa_mainloop* mainLoop_;
	pa_mainloop_api* mainLoopApi_;
	pa_context* context_;
	int retval_;
	PulseDevicePtr currentDevice_;
	QStringList sinksDescriptions_;
	QStringList sourcesDescriptions_;
	QStringList deviceNames_;
	QStringList deviceDescriptions_;
	PulseDevicePtrList sinks_;
	PulseDevicePtrList sources_;
	bool isAvailable_;
};

#endif // PULSECORE_H
