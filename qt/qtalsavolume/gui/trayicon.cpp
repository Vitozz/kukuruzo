/*
 * trayicon.cpp
 * Copyright (C) 2015-2020 Vitaly Tonkacheyev
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
#ifdef KDE_4
#include <kmenu.h>
#endif
#endif

TrayIcon::TrayIcon()
: restore_(new QAction(tr("&Restore"), this)),
  settings_(new QAction(tr("&Settings"), this)),
  mute_(new QAction(tr("&Mute"), this)),
  about_(new QAction(tr("&About..."), this)),
  aboutQt_(new QAction(tr("About Qt"), this)),
  exit_(new QAction(tr("&Quit"), this)),
  trayMenu_(new QMenu()),
  currentIcon_(QString()),
  geometery_(QRect()),
  iconPosition_(QCursor::pos()),
#ifdef HAVE_KDE
  newTrayIcon_(KStatusNotifierItemPtr()),
#endif
  legacyTrayIcon_(QSystemTrayIconPtr())
{
	bool newInterface(false);
#ifdef HAVE_KDE
	QDBusInterface iface(KSNI_SERVICE, KSNI_PATH, KSNI_IFACE);
	if (iface.isValid()) {
		newInterface = true;
		newTrayIcon_ = KStatusNotifierItemPtr(new KStatusNotifierItem(APP_NAME, this));
		newTrayIcon_->setStatus(KStatusNotifierItem::Active);
		newTrayIcon_->setTitle(ICON_TITLE);
#ifdef KDE_4
		newTrayIcon_->setContextMenu(static_cast<KMenu*>(trayMenu_));
#else
		newTrayIcon_->setContextMenu(trayMenu_);
#endif
		connect(newTrayIcon_.data(), &KStatusNotifierItem::activateRequested, this, &TrayIcon::kdeIconActivated);
		connect(newTrayIcon_.data(), &KStatusNotifierItem::secondaryActivateRequested, this, &TrayIcon::iconActivatedSecondary);
		connect(newTrayIcon_.data(), &KStatusNotifierItem::scrollRequested, this, &TrayIcon::onScroll);
	}
#endif
#ifdef ISDEBUG
	qDebug() << "NewIcon " << newInterface;
#endif
	if (!newInterface) {
		legacyTrayIcon_ = QSystemTrayIconPtr(new QSystemTrayIcon(this));
		connect(legacyTrayIcon_.data(), &QSystemTrayIcon::activated,
			this, &TrayIcon::iconActivated);
		legacyTrayIcon_->setContextMenu(trayMenu_);
		legacyTrayIcon_->installEventFilter(this);
	}
	trayMenu_->addAction(restore_.data());
	connect(restore_.data(), &QAction::triggered, this, [this]{
								if (!legacyTrayIcon_) {
									iconPosition_ = QCursor::pos();
								}
								emit activated(RESTORE);});
	trayMenu_->addSeparator();
	trayMenu_->addAction(settings_.data());
	connect(settings_.data(), &QAction::triggered, this, [this]{emit activated(SETTINGS);});
	trayMenu_->addAction(mute_.data());
	mute_->setCheckable(true);
	connect(mute_.data(), &QAction::toggled, this, [this]{emit muted(mute_->isChecked());});
	trayMenu_->addSeparator();
	trayMenu_->addAction(about_.data());
	connect(about_.data(), &QAction::triggered, this, [this]{emit activated(ABOUT);});
	trayMenu_->addAction(aboutQt_.data());
	connect(aboutQt_.data(), &QAction::triggered, this, [this]{emit activated(ABOUTQT);});
	if (legacyTrayIcon_) {
		trayMenu_->addSeparator();
		trayMenu_->addAction(exit_.data());
		connect(exit_.data(), &QAction::triggered, this, [this]{emit activated(EXIT);});
	}
}

TrayIcon::~TrayIcon()
{
	if(legacyTrayIcon_) {
		delete trayMenu_;
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
		QIcon icon_(icon);
		//newTrayIcon_->setIconByName(icon);
		newTrayIcon_->setIconByPixmap(icon_);
		currentIcon_ = icon;
	}
#endif

}

bool TrayIcon::isAvailable()
{
	return legacyTrayIcon_.isNull() || legacyTrayIcon_->isSystemTrayAvailable();
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
void TrayIcon::kdeIconActivated(bool isIt, QPoint point)
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
    if (orientation == Qt::Vertical)
        emit activated(value > 0 ? WHEELUP : WHEELDOWN);
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
		auto wheelEvent = static_cast<QWheelEvent*>(event);
		const int steps = (wheelEvent->delta()>0) ? 1 : (wheelEvent->delta()<0) ? -1: 0;
        emit activated(steps > 0 ? WHEELUP : WHEELDOWN);
        return true;
	}
	return false;
}


QPoint TrayIcon::iconPosition()
{
	return iconPosition_;
}

QRect TrayIcon::iconGeometery()
{
	return geometery_;
}
