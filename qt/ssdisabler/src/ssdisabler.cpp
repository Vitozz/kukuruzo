/*
 * ssdisabler.cpp
 * Copyright (C) 2013-2015 Vitaly Tonkacheyev
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
#include "ssdisabler.h"
#include <QIcon>
#include <QProcess>

SSDisabler::SSDisabler(QWidget *parent)
: QWidget(parent),
  suspended_(false),
  click_(new QAction(tr("Resume"), this)),
  exit_( new QAction(tr("E&xit"), this)),
  trayMenu_(new QMenu(this)),
  trayIcon_(new QSystemTrayIcon(this))

{
	connectActions();
	fillTrayMenu();
	setWindowIcon(QIcon(":/icons/icon.png"));
	trayIcon_->setContextMenu(trayMenu_);
	connect(trayIcon_,
	        SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	        this,
	        SLOT(iconActivated_(QSystemTrayIcon::ActivationReason)));
	winId_ = "0x"+QString::number((int)winId(),16);
	suspend();
	trayIcon_->show();
}

SSDisabler::~SSDisabler()
{
	delete click_;
	delete exit_;
	delete trayMenu_;
	delete trayIcon_;
}

void SSDisabler::connectActions()
{
	connect(click_, SIGNAL(triggered()), this, SLOT(onClick_()));
	connect(exit_, SIGNAL(triggered()), this, SLOT(onExit_()));
}

void SSDisabler::fillTrayMenu()
{
	trayMenu_->addAction(click_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(exit_);
}

void SSDisabler::onExit_()
{
	resume();
	close();
}

void SSDisabler::onClick_()
{
	switchScreensaver();
}

void SSDisabler::switchScreensaver()
{
	if (suspended_) {
		resume();
	}
	else {
		suspend();
	}
}

void SSDisabler::suspend()
{
	QStringList args = QStringList() << "suspend" << winId_;
	startProcess(args);
}

void SSDisabler::resume()
{
	QStringList args = QStringList() << "resume" << winId_;
	startProcess(args);
}

void SSDisabler::startProcess(const QStringList &args)
{
	QProcess cmd;
    QProcess xcs;
	if (args.contains("suspend")) {
		if (cmd.startDetached("xdg-screensaver", args)) {
            if(xcs.startDetached("xset", QStringList() << "s" << "off" << "-dpms")) {
                //
            }
			suspended_ = true;
			click_->setText(tr("Resume"));
			changeTrayIcon();
		}
	}
	else {
		if (cmd.startDetached("xdg-screensaver", args)) {
            if(xcs.startDetached("xset", QStringList() << "s" << "on" << "+dpms")) {
                //
            }
			suspended_ = false;
			click_->setText(tr("Suspend"));
			changeTrayIcon();
		}
	}
}

void SSDisabler::iconActivated_(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		switchScreensaver();
		break;
	default:
	;
	}
}

void SSDisabler::changeTrayIcon()
{
	if (suspended_) {
		trayIcon_->setIcon(QIcon(":/icons/tray_icon_disabled.png"));
		trayIcon_->setToolTip(tr("Screensaver disabled"));
	}
	else {
		trayIcon_->setIcon(QIcon(":/icons/tray_icon_enabled.png"));
		trayIcon_->setToolTip(tr("Screensaver enabled"));
	}
}
