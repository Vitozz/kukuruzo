/*
 * trayicon.h
 * Copyright (C) 2015-2025 Vitaly Tonkacheyev
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

#ifndef TRAYICON_H
#define TRAYICON_H

#include "../defines.h"

#if defined(HAVE_KDE)
#include <KStatusNotifierItem>
#if defined(KDE_4)
#include <kstatusnotifieritem.h>
#endif
#endif
#include <QEvent>
#include <QMenu>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>
#include <QString>
#include <QSystemTrayIcon>

typedef QSharedPointer<QAction> QActionPtr;
#ifdef HAVE_KDE
typedef QSharedPointer<KStatusNotifierItem> KStatusNotifierItemPtr;
#endif
typedef QSharedPointer<QSystemTrayIcon> QSystemTrayIconPtr;

class TrayIcon : public QObject {
  Q_OBJECT

public:
  TrayIcon();
  ~TrayIcon() override;
  void setTrayIcon(const QString &icon);
  void setToolTip(const QString &tooltip);
  void setMute(bool isMuted);
  bool isAvailable();
  QRect iconGeometery();
  QPoint iconPosition();

signals:
  void activated(ActivationReason reason);
  void muted(bool isMuted);

protected slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
  bool eventFilter(QObject *object, QEvent *event) override;

private:
  QActionPtr restore_;
  QActionPtr settings_;
  QActionPtr mute_;
  QActionPtr about_;
  QActionPtr aboutQt_;
  QActionPtr exit_;
  QMenu *trayMenu_;
  QString currentIcon_;
  QRect geometery_;
  QPoint iconPosition_;
#ifdef HAVE_KDE
  KStatusNotifierItemPtr newTrayIcon_;
#endif
  QSystemTrayIconPtr legacyTrayIcon_;
};

#endif // TRAYICON_H
