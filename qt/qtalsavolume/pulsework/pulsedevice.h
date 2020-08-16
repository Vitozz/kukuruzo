/*
 * Copyright (C) 2014-2020 Vitaly Tonkacheyev <thetvg@gmail.com>
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

#ifndef PULSEDEVICE_H
#define PULSEDEVICE_H

#include "pulse/pulseaudio.h"
#include <QString>
#include <QSharedPointer>

enum device_type {
    SOURCE,
    SINK
};

class PulseDevice
{
public:
    PulseDevice();
    explicit PulseDevice(const pa_source_info* i_);
    explicit PulseDevice(const pa_sink_info* i);
    typedef QSharedPointer<PulseDevice> Ptr;
    int index() const;
    int card() const;
    device_type type() const;
    const QString &name() const;
    const QString &description() const;
    pa_cvolume volume;
    int volume_percent();
    bool mute() const;
    static double round(double value) ;
private:
    static int percent(pa_cvolume& volume) ;
private:
    int index_;
    int card_;
    device_type type_;
    QString name_;
    QString description_;
    bool mute_;
};

#endif // PULSEDEVICE_H
