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
#include <QWheelEvent>
#ifdef ISDEBUG
#include <QDebug>
#endif
static const QString ICON_TITLE = "QtAlsaVolume";

#ifdef HAVE_KDE
#include <QDBusInterface>

static const QString KSNI_SERVICE = "org.kde.StatusNotifierWatcher";
static const QString KSNI_PATH = "/StatusNotifierWatcher";
static const QString KSNI_IFACE = "org.kde.StatusNotifierWatcher";
#endif
#ifdef USE_KDE
#include <kmenu.h>
#endif

TrayIcon::TrayIcon()
: restore_(new QAction(tr("&Restore"), this)),
  settings_(new QAction(tr("&Settings"), this)),
  mute_(new QAction(tr("&Mute"), this)),
  about_(new QAction(tr("&About..."), this)),
  aboutQt_(new QAction(tr("About Qt"), this)),
  exit_(new QAction(tr("&Quit"), this)),
  currentIcon_(QString()),
  geometery_(QRect()),
  iconPosition_(QCursor::pos()),
#ifdef HAVE_KDE
  newTrayIcon_(KStatusNotifierItemPtr()),
#endif
  legacyTrayIcon_(QSystemTrayIconPtr())
{
	bool newInterface = false;
	QMenu *trayMenu = new QMenu();
#ifdef HAVE_KDE
	QDBusInterface iface(KSNI_SERVICE, KSNI_PATH, KSNI_IFACE);
	if (iface.isValid()) {
		newInterface = true;
		newTrayIcon_ = KStatusNotifierItemPtr(new KStatusNotifierItem(APP_NAME, this));
		newTrayIcon_->setStatus(KStatusNotifierItem::Active);
		newTrayIcon_->setTitle(ICON_TITLE);
		newTrayIcon_->setContextMenu(static_cast<KMenu*>(trayMenu));
		connect(newTrayIcon_.data(), SIGNAL(activateRequested(bool,QPoint)), this, SLOT(iconActivated(bool,QPoint)));
		connect(newTrayIcon_.data(), SIGNAL(secondaryActivateRequested(QPoint)), this, SLOT(iconActivatedSecondary(QPoint)));
		connect(newTrayIcon_.data(), SIGNAL(scrollRequested(int,Qt::Orientation)), this, SLOT(onScroll(int,Qt::Orientation)));
	}
#endif
#ifdef ISDEBUG
	qDebug() << "NewIcon " << newInterface;
#endif
	if (!newInterface) {
		legacyTrayIcon_ = QSystemTrayIconPtr(new QSystemTrayIcon(this));
		connect(legacyTrayIcon_.data(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
		legacyTrayIcon_->setContextMenu(trayMenu);
		legacyTrayIcon_->installEventFilter(this);
		trayMenu->addAction(restore_.data());
		connect(restore_.data(), SIGNAL(triggered()), this, SLOT(onRestore()));
		trayMenu->addSeparator();
	}
	trayMenu->addAction(settings_.data());
	connect(settings_.data(), SIGNAL(triggered()), this, SLOT(onSettings()));
	trayMenu->addAction(mute_.data());
	mute_->setCheckable(true);
	connect(mute_.data(), SIGNAL(toggled(bool)), this, SLOT(onMute()));
	trayMenu->addSeparator();
	trayMenu->addAction(about_.data());
	connect(about_.data(), SIGNAL(triggered()), this, SLOT(onAbout()));
	trayMenu->addAction(aboutQt_.data());
	connect(aboutQt_.data(), SIGNAL(triggered()), this, SLOT(onAboutQt()));
	if (legacyTrayIcon_) {
		trayMenu->addSeparator();
		trayMenu->addAction(exit_.data());
		connect(exit_.data(), SIGNAL(triggered()), this, SLOT(onExit()));
	}
}

void TrayIcon::setTrayIcon(const QString &icon)
{
	if (legacyTrayIcon_) {
		legacyTrayIcon_->setIcon(QIcon(icon));
		if (!legacyTrayIcon_->isVisible())
			legacyTrayIcon_->show();
	}
#ifdef HAVE_KDE
	else {
		newTrayIcon_->setIconByName(icon);
		currentIcon_ = icon;
	}
#endif

}

bool TrayIcon::isAvailable()
{
	return legacyTrayIcon_.isNull() ? true : legacyTrayIcon_->isSystemTrayAvailable();
}

void TrayIcon::setMute(bool isMuted)
{
	mute_->setChecked(isMuted);
}

void TrayIcon::setToolTip(const QString &tooltip)
{
	if (legacyTrayIcon_) {
		legacyTrayIcon_->setToolTip(tooltip);
	}
#ifdef HAVE_KDE
	else {
		newTrayIcon_->setToolTip(currentIcon_, ICON_TITLE, tooltip);
	}
#endif
}

#ifdef HAVE_KDE
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
#endif
void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		iconPosition_ = QCursor::pos();
		geometery_ = legacyTrayIcon_->geometry();
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
	if (object == legacyTrayIcon_.data() && event->type() == QEvent::Wheel) {
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
	return false;
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