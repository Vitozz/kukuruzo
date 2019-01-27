/*
 * main.cpp
 * Copyright (C) 2013-2019 Vitaly Tonkacheyev
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
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QLocale>
#include "mainwindow.h"

#define ORGANIZATION "thesomeprojects"
#define APPNAME "regexptest"
#define APPVERISON "0.3.6"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QString(ORGANIZATION));
    a.setApplicationName(QString(APPNAME));
    a.setApplicationVersion(QString(APPVERISON));
    QTranslator translator;
    const QStringList localeDirs({QString("%1/langs").arg(QDir::currentPath()),
                                  QString("%1/langs").arg(qApp->applicationDirPath()),
                                  QString("../share/%1/langs").arg(APPNAME),
                                  QString("/usr/share/%1/langs").arg(QString(APPNAME)),
                                  QString("/usr/local/share/%1/langs").arg(QString(APPNAME)),
                                  QString("%1/.local/share/%2/langs").arg(QDir::home().absolutePath(),QString(APPNAME))});
    const QString langFile(APPNAME);
    foreach(const QString &dir, localeDirs){
        if (translator.load(QLocale::system(),langFile, "_", dir )) {
            qApp->installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
