/*
 * alsadevice.h
 * Copyright (C) 2014 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef ALSADEVICE_H
#define ALSADEVICE_H

#include "alsa/asoundlib.h"
#include "mixerswitches.h"
#include <iostream>
#include <QStringList>

class AlsaDevice
{
public:
	AlsaDevice(int id, const QString &card);
	~AlsaDevice();
	AlsaDevice(AlsaDevice const &);
	const QString &name() const;
	int id() const;
	const QStringList &mixers() const;
	const MixerSwitches &switches() const;
	const QString &currentMixer() const;
	int currentMixerId() const;
	bool haveMixers();
	double getVolume();
	bool getMute();
	void setDeviceVolume(double volume);
	void setCurrentMixer(int id);
	void setCurrentMixer(const QString &mixer);
	void setSwitch(const QString &mixer, int id, bool enabled);
	void setMute(bool enabled);

private:
	snd_mixer_t *getMixerHanlde(int id);
	std::string formatCardName(int id) const;
	snd_mixer_selem_channel_id_t checkMixerChannels(snd_mixer_elem_t *element);
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void checkError (int errorIndex);
	void checkError (const QString &title, const QString &message);
	void initMixerList();

private:
	int id_;
	QString name_;
	QStringList volumeMixers_;
	QStringList captureMixers_;
	QStringList mixers_;
	MixerSwitches *switches_;
	int currentMixerId_;
	QString currentMixerName_;

};

#endif // ALSADEVICE_H