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

#include "gui/popupwindow.h"

#include <QApplication>
#include <QSharedMemory>
#include <QTranslator>
#include <QtGui>
#ifdef ISDEBUG
#include <QDebug>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName(APP_ORG);
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setApplicationName(APP_NAME);
    QTranslator translator;
    const QStringList localeDirs({QString("%1/languages").arg(QDir::currentPath()),
                                  QString(qApp->applicationDirPath() + "/languages"),
                                  QString("/usr/share/%1/languages").arg(APP_NAME),
                                  QString("/usr/local/share/%1/languages").arg(APP_NAME),
                                  QString(QDir::home().absolutePath() + "/.local/share/%1/languages").arg(APP_NAME),
                                  QString(QDir::currentPath().left(QDir::currentPath().lastIndexOf("/")) + "/share/%1/languages").arg(APP_NAME)});
    const QString langFile(qApp->applicationName());
    foreach(const QString &dir, localeDirs){
        if (translator.load(QLocale::system(),langFile, "_", dir )) {
            qApp->installTranslator(&translator);
            break;
        }
    }
    QApplication::setQuitOnLastWindowClosed(false);
    PopupWindow w;
    w.hide();
    QSharedMemory sharedMemory;
    sharedMemory.setKey("QtAlsaVolume");
    return (sharedMemory.attach() || !sharedMemory.create(1)) ? 0 : QApplication::exec();
}
