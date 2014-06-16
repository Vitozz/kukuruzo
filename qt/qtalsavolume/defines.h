/*
 * defines.h
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

#ifndef DEFINES_H
#define DEFINES_H

#define APP_NAME "qtalsavolume"
#define APP_ORG "thesomeprojects"
#define APP_VERSION "0.0.6"

#define CARD_INDEX "Main/card"
#define MIXER_NAME "Main/mixer"
#define ISAUTO "Main/autorun"
#define ICOSTYLE "Main/style"
#define LAST_SINK "Pulse/sink"
#define PULSE "Main/pulse"

#include <QPair>

typedef QPair<QString, bool> switcher;

enum Position {
	BOTTOM = 0,
	TOP = 1,
	LEFT = 2,
	RIGHT = 3
};

enum SwitchType {
	PLAYBACK = 0,
	CAPTURE = 1,
	ENUM = 2
};

#endif // DEFINES_H
