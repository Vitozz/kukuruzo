/*
 * main.cpp
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
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>
#include <QDir>
#include <QLocale>

#define APP_ORG "thesomeprojects"
#define APP_VERSION "0.0.1"
#define APP_NAME "ssdisabler"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(APP_ORG);
    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName(APP_NAME);
    QTranslator translator;
    QStringList localeDirs;
    localeDirs << QString(QDir::currentPath());
    localeDirs << QString("%1/langs").arg(QDir::currentPath());
    localeDirs << QString("../share/%1/langs").arg(APP_NAME);
    localeDirs << QString(qApp->applicationDirPath() + "/langs");
    localeDirs << QString("/usr/share/ssdisabler/langs");
    localeDirs << QString("/usr/local/share/ssdisabler/langs");
    localeDirs << QString(QDir::home().absolutePath() + "/.local/share/ssdisabler/langs");
    QString langFile = qApp->applicationName();
    foreach(const QString &dir, localeDirs){
        if (translator.load(QLocale::system(),langFile, "_", dir )) {
            qApp->installTranslator(&translator);
            break;
        }
    }
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Systray"), QObject::tr("System tray not detected"));
        return 1;
    }
    SSDisabler w;
    w.show();
    w.hide();
    QSharedMemory sharedMemory;
    sharedMemory.setKey("SSDisabler");
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
