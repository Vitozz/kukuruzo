/*
 * alsadevice.h
 * Copyright (C) 2014-2019 Vitaly Tonkacheyev
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
 *
 */
#ifndef ALSADEVICE_H
#define ALSADEVICE_H

#include "alsa/asoundlib.h"
#include "mixerswitches.h"
#include <iostream>
#include <QStringList>
#include <QSharedPointer>

class AlsaDevice
{
public:
    AlsaDevice(int id, const QString &card);
    ~AlsaDevice();
    AlsaDevice(AlsaDevice const &);
    typedef QSharedPointer<AlsaDevice> Ptr;
    const QString &name() const;
    int id() const;
    const QStringList &mixers() const;
    MixerSwitches::Ptr switches();
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
    void updateElements();
    static QString formatCardName(int id);

private:
    snd_mixer_t *getMixerHanlde(int id);
    snd_mixer_selem_channel_id_t checkMixerChannels(snd_mixer_elem_t *element);
    snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
    void checkError (int errorIndex);
    void checkError (const QString &title, const QString &message);
    void initMixerList();
    double getNormVolume(snd_mixer_elem_t *element);
    double getExp10(double value) const;
    bool useLinearDb(long min, long max);
    void setNormVolume(snd_mixer_elem_t *element, double volume);

private:
    int id_;
    QString name_;
    QStringList volumeMixers_;
    QStringList captureMixers_;
    QStringList mixers_;
    MixerSwitches::Ptr switches_;
    int currentMixerId_;
    QString currentMixerName_;

};

#endif // ALSADEVICE_H
