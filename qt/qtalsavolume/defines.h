/*
 * defines.h
 * Copyright (C) 2013-2025 Vitaly Tonkacheyev
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

#ifndef DEFINES_H
#define DEFINES_H

#define APP_NAME "qtalsavolume"
#define APP_ORG "thesomeprojects"
#define APP_VERSION "0.2.8"

#define CARD_INDEX "Main/card"
#define MIXER_NAME "Main/mixer"
#define ISAUTO "Main/autorun"
#define ICOSTYLE "Main/style"
#define LAST_SINK "Pulse/sink"
#define PULSE "Main/pulse"
#define ISPOLL "Main/polling"

// Polling timer interval
#define POLLING_INTERVAL 3000

#define DELTA_VOLUME 2

#include <QPair>
#include <QString>

typedef QPair<QString, bool> switcher;

enum Position { BOTTOM = 0, TOP = 1 };

enum SwitchType { PLAYBACK = 0, CAPTURE = 1, ENUM = 2 };
enum ActivationReason {
  RESTORE = 0,
  ABOUT = 1,
  SETTINGS = 2,
  ABOUTQT = 3,
  WHEELUP = 4,
  WHEELDOWN = 5,
  EXIT = 6
};

#endif // DEFINES_H
