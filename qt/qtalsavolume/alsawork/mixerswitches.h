/*
 * mixerswitches.h
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

#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include <QList>
#include <QPair>
#include <QString>

typedef QPair<QString, bool> switcher;

enum SwitchType {
	PLAYBACK = 0,
	CAPTURE = 1,
	ENUM = 2
};

class MixerSwitches
{
public:
	MixerSwitches();
	void pushBack(SwitchType sType, QPair<QString, bool> &item);
	void setCaptureSwitchList(const QList<switcher> &list);
	void setPlaybackSwitchList(const QList<switcher> &list);
	void setEnumSwitchList(const QList<switcher> &list);
	void clear(SwitchType sType);
	QList<switcher> &captureSwitchList();
	QList<switcher> &playbackSwitchList();
	QList<switcher> &enumSwitchList();
	bool isEmpty();

private:
	QList<switcher> captureSwitchList_;
	QList<switcher> playbackSwitchList_;
	QList<switcher> enumSwitchList_;
};

#endif // MIXERSWITCHES_H
