/*
 * alsawork.h
 * Copyright (C) 2012-2025 Vitaly Tonkacheyev
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

#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsadevice.h"
#include "mixerswitches.h"
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

typedef QList<AlsaDevice::Ptr> AlsaDevicePtrList;

class AlsaWork {
public:
  AlsaWork();
  AlsaWork(AlsaWork const &);
  ~AlsaWork();
  typedef QSharedPointer<AlsaWork> Ptr;
  void setCurrentCard(int cardId);
  void setCurrentMixer(int id);
  void setAlsaVolume(int volume);
  int getAlsaVolume();
  static QString getCardName(int index);
  const QStringList &getCardsList() const;
  const QStringList &getVolumeMixers() const;
  MixerSwitches::Ptr getSwitchList() const;
  void setSwitch(const QString &mixer, int id, bool enabled);
  void setMute(bool enabled);
  bool getMute();
  bool haveVolumeMixers();
  bool cardExists(int id) const;
  int getFirstCardWithMixers() const;

private:
  static int getTotalCards();
  static void checkError(int errorIndex);
  void getCards();

private:
  QStringList cardList_;
  int totalCards_;
  AlsaDevice::Ptr currentAlsaDevice_;
  AlsaDevicePtrList devices_;
};
#endif // ALSAWORK_H
