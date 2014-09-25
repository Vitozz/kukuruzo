/*
 * mixerswitches.cpp
 * Copyright (C) 2012-2014 Vitaly Tonkacheyev
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

MixerSwitches::~MixerSwitches()
{
}

MixerSwitches::MixerSwitches(const MixerSwitches &ms)
: captureSwitchList_(ms.captureSwitchList()),
  playbackSwitchList_(ms.playbackSwitchList()),
  enumSwitchList_(ms.enumSwitchList())
{
}

void MixerSwitches::setCaptureSwitchList(const QList<switcher> &list)
{
	captureSwitchList_ = list;
}

void MixerSwitches::setPlaybackSwitchList(const QList<switcher> &list)
{
	playbackSwitchList_= list;
}

void MixerSwitches::setEnumSwitchList(const QList<switcher> &list)
{
	enumSwitchList_ = list;
}

void MixerSwitches::pushBack(SwitchType sType, switcher &item)
{
	switch (sType) {
	case PLAYBACK:
		playbackSwitchList_ << item;
		break;
	case CAPTURE:
		captureSwitchList_ << item;
		break;
	case ENUM:
		enumSwitchList_ << item;
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

const QList<switcher> &MixerSwitches::captureSwitchList() const
{
	return captureSwitchList_;
}

const QList<switcher> &MixerSwitches::playbackSwitchList() const
{
	return playbackSwitchList_;
}

const QList<switcher> &MixerSwitches::enumSwitchList() const
{
	return enumSwitchList_;
}

bool MixerSwitches::isEmpty()
{
	return (captureSwitchList_.isEmpty() && playbackSwitchList_.isEmpty() && enumSwitchList_.isEmpty());
}
