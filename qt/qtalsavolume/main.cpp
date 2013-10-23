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
#include <QApplication>
#include <QTranslator>
#include <QtGui>

#define APP_NAME "qtalsavolume"
#define APP_ORG "thesomeprojects"
#define APP_VERSION "0.0.1"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName(APP_ORG);
	a.setApplicationVersion(APP_VERSION);
	a.setApplicationName(APP_NAME);
	QTranslator translator;
	QStringList localeDirs;
	localeDirs << QString("%1/languages").arg(QDir::currentPath());
	localeDirs << QString(qApp->applicationDirPath() + "/languages");
	localeDirs << QString("/usr/share/qtalsavolume/languages");
	localeDirs << QString("/usr/local/share/qtalsavolume/languages");
	localeDirs << QString(QDir::home().absolutePath() + "/.local/share/qtalsavolume/languages");
	QString langFile = qApp->applicationName();
	foreach(QString dir, localeDirs){
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
	return a.exec();
}
