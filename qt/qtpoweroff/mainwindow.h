/*
 * mainwindow.h
 * Copyright (C) 2014-2019 Vitaly Tonkacheyev
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDateTime>
#include <QSharedPointer>
#ifdef HAVE_DBUS
#include <QDBusMessage>
#include <QDBusInterface>
#endif

typedef QSharedPointer<QDateTime> QDateTimePtr;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected slots:
    void onExit();
    void onTerminate();
    void onAbout();
    void onTimeChecked(bool toggled);
    void onMinsChecked(bool toggled);
    void onMinSpin(int mins);
    void onTimeChange(const QDateTime &time);
    void onRebootChecked(bool toggled);
    void onShutdownChecked(bool toggled);
    void onPowerOffClicked();
    void onTimer();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
    void changeEvent(QEvent *e) override;
    void closeEvent(QCloseEvent *) override;

private:
    void SetTrayToolTip(const QString &text);
    void ReadSettings();
    void SaveSettings();
    void initActions();
    void updateTrayMenu();
    void doAction();
    void calculateTimeOffset();
    void showTrayMessage(const QString &msg);
    QString getTimeString(uint seconds) const;
#ifdef HAVE_DBUS
    bool isIfaceAvailable(const QString &service, const QString &path, const QString &iface);
    bool doDbusAction(const QString &service, const QString &path, const QString &iface);
#endif
#ifdef Q_OS_WIN
    void doProcessRun();
    bool doShutdown();
#endif

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon_;
    QTimer *poffTimer_;
    QDateTimePtr time_;
    QAction *terminate_;
    QAction *about_;
    QAction *aboutQt_;
    QAction *exit_;
    QMenu *popupMenu_;
    bool isMinutes_;
    bool isReboot_;
    int offset_;
};

#endif // MAINWINDOW_H
