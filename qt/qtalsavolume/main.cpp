/*
 * main.cpp
 * Copyright (C) 2013 Vitaly Tonkacheyev
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

#include "popupwindow.h"
#include "defines.h"

#include <QApplication>
#include <QSharedMemory>
#include <QTranslator>
#include <QtGui>
#include <QMessageBox>
#ifdef ISDEBUG
#include <QDebug>
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName(APP_ORG);
	a.setApplicationVersion(APP_VERSION);
	a.setApplicationName(APP_NAME);
	QTranslator translator;
	const QStringList localeDirs = QStringList() << QString("%1/languages").arg(QDir::currentPath())
						     << QString(qApp->applicationDirPath() + "/languages")
						     << QString("/usr/share/qtalsavolume/languages")
						     << QString("/usr/local/share/qtalsavolume/languages")
						     << QString(QDir::home().absolutePath() + "/.local/share/qtalsavolume/languages");
	const QString langFile = qApp->applicationName();
	foreach(const QString &dir, localeDirs){
		if (translator.load(QLocale::system(),langFile, "_", dir )) {
			qApp->installTranslator(&translator);
			break;
		}
	}
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("System tray not detected"));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);
	PopupWindow w;
	w.show();
	w.hide();
	QSharedMemory sharedMemory;
	sharedMemory.setKey("QtAlsaVolume");
	if (sharedMemory.attach()) {
		return 0;
	}
	if (!sharedMemory.create(1)) {
		return 0;
	}
	else{
		return a.exec();
	}
}
