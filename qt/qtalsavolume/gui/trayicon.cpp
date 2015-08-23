/*
 * trayicon.cpp
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

#include "trayicon.h"
#ifndef USE_KDE5
#include <QWheelEvent>
#endif
#ifdef ISDEBUG
#include <QDebug>
#endif
static const QString ICON_TITLE = "QtAlsaVolume";

TrayIcon::TrayIcon()
:
#ifdef USE_KDE5
  trayIcon_(new KStatusNotifierItem(APP_NAME, this)),
#else
  trayIcon_(new QSystemTrayIcon(this)),
#endif
  trayMenu_(new QMenu()),
  restore_(new QAction(tr("&Restore"), trayMenu_)),
  settings_(new QAction(tr("&Settings"), trayMenu_)),
  mute_(new QAction(tr("&Mute"), trayMenu_)),
  about_(new QAction(tr("&About..."), trayMenu_)),
  aboutQt_(new QAction(tr("About Qt"), trayMenu_)),
  exit_(new QAction(tr("&Quit"), trayMenu_)),
  currentIcon_(QString()),
  geometery_(QRect()),
  iconPosition_(QCursor::pos())
{
	connect(restore_, SIGNAL(triggered()), this, SLOT(onRestore()));
	connect(settings_, SIGNAL(triggered()), this, SLOT(onSettings()));
	mute_->setCheckable(true);
	connect(mute_, SIGNAL(toggled(bool)), this, SLOT(onMute()));
	connect(about_, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(aboutQt_, SIGNAL(triggered()), this, SLOT(onAboutQt()));
	connect(exit_, SIGNAL(triggered()), this, SLOT(onExit()));
	trayMenu_->addAction(restore_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(settings_);
	trayMenu_->addAction(mute_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(about_);
	trayMenu_->addAction(aboutQt_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(exit_);
#ifdef USE_KDE5
	trayIcon_->setStatus(KStatusNotifierItem::Active);
	trayIcon_->setTitle(ICON_TITLE);
	trayIcon_->setContextMenu(trayMenu_);
	connect(trayIcon_, SIGNAL(activateRequested(bool,QPoint)), this, SLOT(iconActivated(bool,QPoint)));
	connect(trayIcon_, SIGNAL(secondaryActivateRequested(QPoint)), this, SLOT(iconActivatedSecondary(QPoint)));
	connect(trayIcon_, SIGNAL(scrollRequested(int,Qt::Orientation)), this, SLOT(onScroll(int,Qt::Orientation)));
#else
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	trayIcon_->setContextMenu(trayMenu_);
	installEventFilter(this);
#endif

}

TrayIcon::~TrayIcon()
{
	disconnectAll();
	delete trayIcon_;
}

void TrayIcon::disconnectAll()
{
	trayIcon_->disconnect();
	disconnect(exit_, 0, 0, 0);
	disconnect(aboutQt_, 0, 0, 0);
	disconnect(about_, 0, 0, 0);
	disconnect(mute_, 0, 0, 0);
	disconnect(settings_, 0, 0, 0);
	disconnect(restore_, 0, 0, 0);
}

void TrayIcon::setTrayIcon(const QString &icon)
{
#ifdef USE_KDE5
	trayIcon_->setIconByName(icon);
	currentIcon_ = icon;
#else
	trayIcon_->setIcon(QIcon(icon));
	if (!trayIcon_->isVisible())
		trayIcon_->show();
#endif
}

bool TrayIcon::isAvailable()
{
#ifdef USE_KDE5
	return true;
#else
	return trayIcon_->isSystemTrayAvailable();
#endif
}

void TrayIcon::setMute(bool isMuted)
{
	mute_->setChecked(isMuted);
}

void TrayIcon::setToolTip(const QString &tooltip)
{
#ifdef USE_KDE5
	trayIcon_->setToolTip(currentIcon_, ICON_TITLE, tooltip);
#else
	trayIcon_->setToolTip(tooltip);
#endif
}

void TrayIcon::onAbout()
{
	emit activated(ABOUT);
}

void TrayIcon::onAboutQt()
{
	emit activated(ABOUTQT);
}

void TrayIcon::onExit()
{
	emit activated(EXIT);
}

void TrayIcon::onMute()
{
	emit muted(mute_->isChecked());
}

void TrayIcon::onRestore()
{
	emit activated(RESTORE);
}

void TrayIcon::onSettings()
{
	emit activated(SETTINGS);
}

QPoint TrayIcon::iconPosition()
{
	return iconPosition_;
}

QRect TrayIcon::iconGeometery()
{
	return geometery_;
}

#ifdef USE_KDE5
void TrayIcon::iconActivated(bool isIt, QPoint point)
{
	if (isIt) {
		iconPosition_ = point;
#ifdef ISDEBUG
		qDebug() << "POS " << point;
#endif
		emit activated(RESTORE);
	}
}

void TrayIcon::iconActivatedSecondary(QPoint point)
{
	Q_UNUSED(point);
	setMute(!mute_->isChecked());
}


void TrayIcon::onScroll(int value, Qt::Orientation orientation)
{
	if (orientation == Qt::Vertical) {
		if (value > 0) {
			emit activated(WHEELUP);
		}
		else {
			emit activated(WHEELDOWN);
		}
	}
}
#else
void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		iconPosition_ = QCursor::pos();
		geometery_ = this->iconGeometery();
		emit activated(RESTORE);
		break;
	case QSystemTrayIcon::MiddleClick:
		setMute(!mute_->isChecked());
		break;
	default:
		break;
	}
}

bool TrayIcon::eventFilter(QObject *object, QEvent *event)
{
	if (object == trayIcon_) {
		if (event->type() == QEvent::Wheel) {
			QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
			const int steps = (wheelEvent->delta()>0) ? 1 : (wheelEvent->delta()<0) ? -1: 0;
			if (steps > 0) {
				emit activated(WHEELUP);
			}
			else {
				emit activated(WHEELDOWN);
			}
			return true;
		}
	}
	return false;
}
#endif
