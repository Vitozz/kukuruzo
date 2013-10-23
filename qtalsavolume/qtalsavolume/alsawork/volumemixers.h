/*
 * volumemixers.h
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

#ifndef VOLUMEMIXERS_H
#define VOLUMEMIXERS_H

#include <QString>
#include <QStringList>

class VolumeMixers
{
public:
	VolumeMixers();
	QStringList &playback();
	QStringList &capture();
	void setPlayback(const QStringList &list);
	void setCapture(const QStringList &list);
	void playBackClear();
	void captureClear();
	void pushBack(int mixerType, const QString &item);
private:
	QStringList playback_;
	QStringList capture_;
};

#endif // VOLUMEMIXERS_H
