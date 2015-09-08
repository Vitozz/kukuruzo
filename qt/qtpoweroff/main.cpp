/*
 * main.cpp
 * Copyright (C) 2014 Vitaly Tonkacheyev
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
#include "defines.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QDir>
#include <QLocale>
#ifdef IS_DEBUG
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
						     << QString("%1/languages").arg(qApp->applicationDirPath())
						     << QString("/usr/share/%1/languages").arg(APP_NAME)
						     << QString("/usr/local/share/%1/languages").arg(APP_NAME)
						     << QString("%1/.local/share/%2/languages").arg(QDir::home().absolutePath(), APP_NAME)
						     << QString("%1/share/%2/languages").arg(QDir::currentPath().left(QDir::currentPath().lastIndexOf("/")), APP_NAME);
	const QString langFile(qApp->applicationName());
	foreach(const QString &dir, localeDirs){
#ifdef IS_DEBUG
			qDebug() << "Dir is " << dir;
#endif
		if (translator.load(QLocale::system(),langFile, "_", dir )) {
			qApp->installTranslator(&translator);
#ifdef IS_DEBUG
			qDebug() << "Found in " << dir;
#endif
			break;
		}
	}
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("System tray not detected\nExiting..."));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);
	MainWindow w;
	w.show();

	return a.exec();
}
