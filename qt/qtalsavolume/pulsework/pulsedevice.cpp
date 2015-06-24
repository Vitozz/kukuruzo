/*
 * Copyright (C) 2014-2015 Vitaly Tonkacheyev <thetvg@gmail.com>
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

#include "pulsedevice.h"
#include <QtCore/qmath.h>
#ifdef ISDEBUG
#include <QDebug>
#endif
#define ALSA_CARDID_PROPERTY "alsa.card"

static int getCardId(pa_proplist *pl)
{
	const QString cardId = pa_proplist_gets(pl, ALSA_CARDID_PROPERTY);
	return (!cardId.isNull()) ? cardId.toInt() : 0;
}

PulseDevice::PulseDevice()
: index_(0),
  card_(0),
  type_(SINK),
  name_(QString()),
  description_(QString()),
  mute_(false)
{
}

PulseDevice::PulseDevice(const pa_source_info* i)
: index_(i->index),
  card_(getCardId(i->proplist)),
  type_(SOURCE),
  name_(QString(i->name)),
  description_(QString::fromLocal8Bit(i->description))
{
	volume.channels = i->volume.channels;
	int n;
	for (n = 0; n < volume.channels; ++n) {
		volume.values[n] = i->volume.values[n];
	}
	mute_ = i->mute == 1;
}

PulseDevice::PulseDevice(const pa_sink_info* i)
: index_(i->index),
  card_(getCardId(i->proplist)),
  type_(SINK),
  name_(QString(i->name)),
  description_(QString::fromLocal8Bit(i->description))
{
	volume.channels = i->volume.channels;
	int n;
	for (n = 0; n < volume.channels; ++n) {
		volume.values[n] = i->volume.values[n];
	}
	mute_ = i->mute == 1;
}

int PulseDevice::percent(pa_cvolume& volume_) const
{
	return (int) round(((double) pa_cvolume_avg(&volume_) * 100.) / PA_VOLUME_NORM);
}

double PulseDevice::round(double value) const
{
	return (value > 0.0) ? qFloor(value + 0.5) : qCeil(value - 0.5);
}

uint32_t PulseDevice::index() const
{
	return index_;
}

uint32_t PulseDevice::card() const
{
	return card_;
}

device_type PulseDevice::type() const
{
	return type_;
}

const QString &PulseDevice::name() const
{
	return name_;
}

const QString &PulseDevice::description() const
{
	return description_;
}

int PulseDevice::volume_percent()
{
	return percent(volume);
}

bool PulseDevice::mute() const
{
	return mute_;
}

