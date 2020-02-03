/*
 * mixerswitches.h
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
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

#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include "defines.h"
#include <QList>
#include <QSharedPointer>

class MixerSwitches
{
public:
    MixerSwitches() = default;
    MixerSwitches(const MixerSwitches &ms);
    ~MixerSwitches() = default;
    typedef QSharedPointer<MixerSwitches> Ptr;
    void pushBack(SwitchType sType, switcher &item);
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
