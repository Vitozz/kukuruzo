/*
 * trayicon.h
 * Copyright (C) 2015 Vitaly Tonkacheyev
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

#ifndef TRAYICON_H
#define TRAYICON_H

#include "../defines.h"

#ifdef USE_KDE5
#include <KStatusNotifierItem>
#endif
#include <QSharedPointer>
#include <QSystemTrayIcon>
#include <QEvent>
#include <QString>
#include <QRect>
#include <QPoint>
#include <QMenu>

typedef QSharedPointer<QAction> QActionPtr;
#ifdef USE_KDE5
typedef QSharedPointer<KStatusNotifierItem> KStatusNotifierItemPtr;
#endif
typedef QSharedPointer<QSystemTrayIcon> QSystemTrayIconPtr;


class TrayIcon : public QObject
{
	Q_OBJECT

public:
	TrayIcon();
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
	void onAbout();
	void onAboutQt();
	void onRestore();
	void onMute();
	void onSettings();
	void onExit();
#ifdef USE_KDE5
	void iconActivated(bool isIt, QPoint point);
	void iconActivatedSecondary(QPoint point);
	void onScroll(int value,Qt::Orientation orientation);
#endif
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
	bool eventFilter(QObject *object, QEvent *event);

private:
	QMenu *trayMenu_;
	QActionPtr restore_;
	QActionPtr settings_;
	QActionPtr mute_;
	QActionPtr about_;
	QActionPtr aboutQt_;
	QActionPtr exit_;
	QString currentIcon_;
	QRect geometery_;
	QPoint iconPosition_;
#ifdef USE_KDE5
	KStatusNotifierItemPtr newTrayIcon_;
#endif
	QSystemTrayIconPtr legacyTrayIcon_;
};

#endif // TRAYICON_H
