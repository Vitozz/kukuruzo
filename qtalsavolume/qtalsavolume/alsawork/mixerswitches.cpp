/*
 * mixerswitches.cpp
 * Copyright (C) 2012 Vitaly Tonkacheyev
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

#include "mixerswitches.h"

MixerSwitches::MixerSwitches()
{
}

void MixerSwitches::setCaptureSwitchList(const QList<switcher> &list)
{
	captureSwitchList_.append(list);
}

void MixerSwitches::setPlaybackSwitchList(const QList<switcher> &list)
{
	playbackSwitchList_.append(list);
}

void MixerSwitches::setEnumSwitchList(const QList<switcher> &list)
{
	enumSwitchList_.append(list);
}

void MixerSwitches::pushBack(SwitchType sType, switcher &item)
{
	switch (sType) {
	case PLAYBACK:
		playbackSwitchList_.push_back(item);
		break;
	case CAPTURE:
		captureSwitchList_.push_back(item);
		break;
	case ENUM:
		enumSwitchList_.push_back(item);
		break;
	}
}

void MixerSwitches::clear(SwitchType sType)
{
	switch (sType) {
	case PLAYBACK:
		if (!playbackSwitchList_.isEmpty())
			playbackSwitchList_.clear();
		break;
	case CAPTURE:
		if (!captureSwitchList_.isEmpty())
			captureSwitchList_.clear();
		break;
	case ENUM:
		if (!enumSwitchList_.isEmpty())
			enumSwitchList_.clear();
		break;
	}
}

QList<switcher> &MixerSwitches::captureSwitchList()
{
	return captureSwitchList_;
}

QList<switcher> &MixerSwitches::playbackSwitchList()
{
	return playbackSwitchList_;
}

QList<switcher> &MixerSwitches::enumSwitchList()
{
	return enumSwitchList_;
}

bool MixerSwitches::isEmpty()
{
	bool result = bool(playbackSwitchList().isEmpty() && captureSwitchList().isEmpty() && enumSwitchList().isEmpty());
	return result;
}
