/*
 * volumemixers.cpp
 * Copyright (C) 2013 Vitaly Tonkacheyev
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

#include "volumemixers.h"

VolumeMixers::VolumeMixers()
{
}

QStringList &VolumeMixers::playback()
{
	return playback_;
}

QStringList &VolumeMixers::capture()
{
	return capture_;
}

void VolumeMixers::setPlayback(const QStringList &list)
{
	playback_.append(list);
}

void VolumeMixers::setCapture(const QStringList &list)
{
	capture_.append(list);
}

void VolumeMixers::playBackClear()
{
	if (!playback_.isEmpty())
		playback_.clear();
}

void VolumeMixers::captureClear()
{
	if (!capture_.isEmpty())
		capture_.clear();
}

void VolumeMixers::pushBack(int mixerType, const QString &item)
{
	switch (mixerType) {
	case 0:
		playback_.push_back(item);
		break;
	case 1:
		capture_.push_back(item);
		break;
	}
}
