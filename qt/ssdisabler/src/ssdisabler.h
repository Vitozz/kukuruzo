/*
 * ssdisabler.h
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
#ifndef SSDISABLER_H
#define SSDISABLER_H

#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>

class SSDisabler : public QWidget
{
    Q_OBJECT

public:
    SSDisabler(QWidget *parent = nullptr);
    ~SSDisabler();

protected slots:
    void onClick_();
    void onExit_();
    void iconActivated_(QSystemTrayIcon::ActivationReason reason);

private:
    void connectActions();
    void fillTrayMenu();
    void changeTrayIcon();
    void switchScreensaver();
    void suspend();
    void resume();
    void startProcess(const QStringList &args);

private:
    bool suspended_;
    QAction *click_;
    QAction *exit_;
    QMenu *trayMenu_;
    QSystemTrayIcon *trayIcon_;
    QString winId_;
};

#endif // SSDISABLER_H
