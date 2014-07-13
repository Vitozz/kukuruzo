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

#ifndef PULSECORE_H
#define PULSECORE_H

#include "pulsedevice.h"
#include "pulse/pulseaudio.h"
#include <QStringList>
#include <QList>
#include <QPair>

struct ServerInfo {
	QString defaultSourceName;
	QString defaultSinkName;
};
enum state {
	CONNECTING,
	CONNECTED,
	ERROR
};

class PulseCore
{
public:
	PulseCore(const char *clientName);
	~PulseCore();
	state pState;
	QString defaultSink();
	QStringList getCardList();
	QString getDeviceDescription(const QString &name);
	QString getDeviceNameByIndex(int index);
	int getCurrentDeviceIndex();
	int getVolume();
	bool getMute();
	void setVolume(int value);
	void setMute(bool mute);
	void setCurrentDevice(const QString &name);
private:
	QList<PulseDevice> getSinks();
	QList<PulseDevice> getSources();
	PulseDevice getSink(u_int32_t);
	PulseDevice getSink(const QString &name);
	PulseDevice getSource(u_int32_t);
	PulseDevice getSource(const QString &name);
	PulseDevice getDefaultSink();
	PulseDevice getDefaultSource();
	PulseDevice getDeviceByName(const QString &name);
	PulseDevice getDeviceByIndex(int index);
	QStringList getSinksDescriptions();
	QStringList getSourcesDescriptions();
	void setVolume_(PulseDevice &device, int value);
	void setMute_(PulseDevice &device, bool mute);
	void iterate(pa_operation* op);
	void onError(const QString &message);
private:
	pa_mainloop* mainLoop_;
	pa_mainloop_api* mainLoopApi_;
	pa_context* context_;
	int retval_;
	PulseDevice *currentDevice_;
};

#endif // PULSECORE_H
