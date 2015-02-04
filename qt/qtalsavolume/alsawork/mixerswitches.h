/*
 * mixerswitches.h
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

#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include "defines.h"
#include <QList>

class MixerSwitches
{
public:
	MixerSwitches();
	MixerSwitches(const MixerSwitches &ms);
	~MixerSwitches();
	void pushBack(SwitchType sType, switcher &item);
	void setCaptureSwitchList(const QList<switcher> &list);
	void setPlaybackSwitchList(const QList<switcher> &list);
	void setEnumSwitchList(const QList<switcher> &list);
	void clear(SwitchType sType);
	void clearAll();
	const QList<switcher> &captureSwitchList() const;
	const QList<switcher> &playbackSwitchList() const;
	const QList<switcher> &enumSwitchList() const;
	bool isEmpty();

private:
	QList<switcher> captureSwitchList_;
	QList<switcher> playbackSwitchList_;
	QList<switcher> enumSwitchList_;
};

#endif // MIXERSWITCHES_H
