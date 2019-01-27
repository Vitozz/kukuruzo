/*
 * mainwindow.cpp
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defines.h"

#include <QMessageBox>
#include <QSettings>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QAction>
#ifdef HAVE_DBUS
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#endif
#ifdef Q_OS_WIN
#include "qt_windows.h"
#include <QProcess>
#endif
#ifdef IS_DEBUG
#include <QDebug>
#endif

static const int secsInDay = 24*3600;
static const int secsInHour = 3600;
static const int secsInMin = 60;

static const QString CK_SERVICE = "org.freedesktop.ConsoleKit";
static const QString CK_PATH = "/org/freedesktop/ConsoleKit/Manager";
static const QString CK_IFACE = "org.freedesktop.ConsoleKit.Manager";
static const QString CK_REBOOT = "Restart";
static const QString CK_POWEROFF = "Stop";

static const QString LD_SERVICE = "org.freedesktop.login1";
static const QString LD_PATH = "/org/freedesktop/login1";
static const QString LD_IFACE= "org.freedesktop.login1.Manager";
static const QString LD_REBOOT = "Reboot";
static const QString LD_POWEROFF = "PowerOff";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      trayIcon_(new QSystemTrayIcon(QIcon(":/images/32x32.png"), this)),
      poffTimer_(new QTimer(this)),
      time_(QDateTimePtr(new QDateTime())),
      terminate_(new QAction(tr("Terminate"), this)),
      about_(new QAction(tr("About %1").arg(APP_TITLE), this)),
      aboutQt_(new QAction(tr("About Qt"), this)),
      exit_(new QAction(tr("Exit"), this)),
      popupMenu_(new QMenu(this)),
      isMinutes_(true),
      isReboot_(true),
      offset_(0)
{
    ui->setupUi(this);
    initActions();
    updateTrayMenu();
    trayIcon_->setContextMenu(popupMenu_);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionTerminate, &QAction::triggered, this, &MainWindow::onTerminate);
    connect(ui->byMinutesRadio, &QRadioButton::toggled, this, &MainWindow::onMinsChecked);
    connect(ui->byTimeRadio, &QRadioButton::toggled, this, &MainWindow::onTimeChecked);
    connect(ui->isReboot, &QRadioButton::toggled, this, &MainWindow::onRebootChecked);
    connect(ui->isShutdown, &QRadioButton::toggled, this, &MainWindow::onShutdownChecked);
    connect(ui->minutesSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onMinSpin);
    connect(ui->poweroffBtn, &QPushButton::clicked, this, &MainWindow::onPowerOffClicked);
    connect(ui->dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::onTimeChange);
    connect(poffTimer_, &QTimer::timeout, this, &MainWindow::onTimer);
    connect(trayIcon_, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    trayIcon_->show();
    ReadSettings();
    SetTrayToolTip(QString("%1-%2").arg(APP_TITLE, APP_VERSION));
    ui->actionTerminate->setEnabled(false);
    terminate_->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete popupMenu_;
    delete exit_;
    delete aboutQt_;
    delete about_;
    delete poffTimer_;
    delete trayIcon_;
}

void MainWindow::initActions()
{
    connect(terminate_, &QAction::triggered, this, &MainWindow::onTerminate);
    connect(about_, &QAction::triggered, this, &MainWindow::onAbout);
    connect(aboutQt_, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(exit_, &QAction::triggered, this, &MainWindow::onExit);
}

void MainWindow::updateTrayMenu()
{
    popupMenu_->addAction(terminate_);
    popupMenu_->addSeparator();
    popupMenu_->addAction(about_);
    popupMenu_->addAction(aboutQt_);
    popupMenu_->addSeparator();
    popupMenu_->addAction(exit_);
}

void MainWindow::SetTrayToolTip(const QString &text)
{
    trayIcon_->setToolTip(text);
}

void MainWindow::onExit()
{
    SaveSettings();
    qApp->quit();
}

void MainWindow::onMinsChecked(bool toggled)
{
    ui->dateTimeEdit->setEnabled(!toggled);
    isMinutes_ = toggled;
}

void MainWindow::onAbout()
{
    const QString message = tr("<!DOCTYPE html><html><body>"
                               "<p><b>Tool for scheduled Shutdown/Reboot</b></p>"
                               "<p>written using Qt</p>"
                               "<p>2018 (c) Vitaly Tonkacheyev <address><a href=\"mailto:thetvg@gmail.com\">&lt;EMail&gt;</a></address></p>"
                               "<a href=\"http://sites.google.com/site/thesomeprojects/\">Program WebSite</a>"
                               "<p>version: <b>%1</b></p></body></html>").arg(APP_VERSION);
    QMessageBox aboutDialog;
    aboutDialog.setWindowTitle(tr("About %1").arg(APP_TITLE));
    aboutDialog.setText(message);
    aboutDialog.setIconPixmap(QPixmap(":/images/poweroff.png"));
    aboutDialog.setWindowIcon(this->windowIcon());
    aboutDialog.exec();
}

void MainWindow::onMinSpin(int mins)
{
    offset_ = mins*secsInMin;
}

QString MainWindow::getTimeString(uint seconds) const
{
    uint secs = seconds;
    const uint day = secs / secsInDay;
    secs -= day * secsInDay;
    const uint hour = secs / secsInHour;
    secs -= hour * secsInHour;
    const uint mins = secs / secsInMin;
    secs -= mins * secsInMin;
    const QString dayStr((day >= 10) ? QString::number(day) : "0" + QString::number(day));
    const QString hourStr((hour >= 10) ? QString::number(hour) : "0" + QString::number(hour));
    const QString minStr((mins >= 10) ? QString::number(mins) : "0" + QString::number(mins));
    const QString secStr((secs >= 10) ? QString::number(secs) : "0" + QString::number(secs));
    if (day == 0) {
        return (tr("%1:%2:%3").arg(hourStr, minStr, secStr));
    }
    return (tr("%1 days %2:%3:%4").arg(dayStr, hourStr, minStr, secStr));
}

void MainWindow::calculateTimeOffset()
{
    if (isMinutes_) {
        time_ = QDateTimePtr(new QDateTime(time_->currentDateTime().addSecs(offset_)));
#ifdef IS_DEBUG
        qDebug() << "Time - " << time_->toString("dd-MM-yyyy hh:mm:ss");
        qDebug() << "Offset " << offset_;
#endif
    }
    else {
        if(!time_->isNull()) {
            offset_ = int(time_->currentDateTime().secsTo(*time_));
#ifdef IS_DEBUG
            qDebug() << "Time - " << time_->toString("dd-MM-yyyy hh:mm:ss");
            qDebug() << "Offset " << offset_;
#endif
        }
    }
}

void MainWindow::showTrayMessage(const QString &msg)
{
    const QString title((isReboot_) ? tr("Reboot") : tr("Shutdown"));
    trayIcon_->showMessage(title, msg);
}

void MainWindow::onPowerOffClicked()
{
    if (!poffTimer_->isActive()) {
        calculateTimeOffset();
        const QString timeStr(tr("%1 at %2").arg(time_->toString("dd-MM-yyyy"),
                                                 time_->toString("hh:mm:ss")));
        QMessageBox box;
        const QString poffType((isReboot_) ? tr("reboot") : tr("shutdown"));
        box.setWindowTitle(tr("Please Confirm Your Choice"));
        box.setIcon(QMessageBox::Question);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.setText(tr("Do You realy whant to %1 your PC\n %2").arg(poffType, timeStr));
        if (box.exec() == QMessageBox::Ok) {
            ui->actionTerminate->setEnabled(true);
            terminate_->setEnabled(true);
            showTrayMessage(timeStr);
            poffTimer_->start(TIME_INTERVAL);
        }
    }
}

void MainWindow::onRebootChecked(bool toggled)
{
    isReboot_ = toggled;
}

void MainWindow::onShutdownChecked(bool toggled)
{
    isReboot_ = !toggled;
}

void MainWindow::onTerminate()
{
    if (poffTimer_->isActive()) {
#ifdef IS_DEBUG
        qDebug() << "Timer action terminated";
#endif
        poffTimer_->stop();
        ui->actionTerminate->setEnabled(false);
        terminate_->setEnabled(false);
        setWindowTitle(APP_TITLE);
        showTrayMessage(tr("Terminated"));
    }
}

void MainWindow::onTimeChange(const QDateTime &time)
{
    time_= QDateTimePtr(new QDateTime(time));
}

void MainWindow::onTimeChecked(bool toggled)
{
    ui->minutesSpin->setEnabled(!toggled);
    isMinutes_ = !toggled;
}

void MainWindow::onTimer()
{
    uint deltaTime = time_->toTime_t() - time_->currentDateTime().toTime_t();//currentTime->toTime_t();
    if (deltaTime == 0 || offset_ == 0) {
        doAction();
#ifdef IS_DEBUG
        qDebug() << "action";
#endif
    }
    else {
        const QString timeStr(getTimeString(deltaTime));
#ifdef IS_DEBUG
        qDebug() << "T - " << timeStr;
#endif
        const QString poffType((isReboot_) ? tr("Reboot") : tr("Shutdown"));
        const QString tip(tr("%1 at - %2").arg(poffType, timeStr));
        SetTrayToolTip(tip);
        setWindowTitle(QString("%1 - %2").arg(APP_TITLE, tip));
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    SaveSettings();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isHidden()) {
            show();
        }
        else {
            hide();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
        onTerminate();
        break;
    default:
        ;
    }
}

void MainWindow::ReadSettings()
{
    QSettings settings_;
    const int mins = settings_.value(MINS, 0).toInt();
    const int hours = settings_.value(HOURS, 0).toInt();
    const int alltime = settings_.value(POFF_TIME, 0).toInt();
    int day = settings_.value(DAYS, 0).toInt();
    QDate date = QDate(QDate::currentDate().year(), QDate::currentDate().month(), day);
    ui->minutesSpin->setValue(alltime);
    ui->dateTimeEdit->setDateTime(QDateTime(date, QTime(hours, mins)));
    bool timeType = settings_.value(TIME_TYPE, false).toBool();
    ui->byMinutesRadio->setChecked(timeType);
    ui->byTimeRadio->setChecked(!timeType);
    onMinsChecked(timeType);
    bool poffType = settings_.value(POFF_TYPE, false).toBool();
    ui->isReboot->setChecked(poffType);
    ui->isShutdown->setChecked(!poffType);
    onRebootChecked(poffType);
}

void MainWindow::SaveSettings()
{
    QSettings settings_;
    settings_.setValue(TIME_TYPE, ui->byMinutesRadio->isChecked());
    settings_.setValue(POFF_TYPE, ui->isReboot->isChecked());
    settings_.setValue(POFF_TIME, ui->minutesSpin->value());
    settings_.setValue(MINS, ui->dateTimeEdit->time().minute());
    settings_.setValue(HOURS, ui->dateTimeEdit->time().hour());
    settings_.setValue(DAYS, ui->dateTimeEdit->date().day());
}

void MainWindow::doAction()
{
    onTerminate();
#ifdef HAVE_DBUS
    bool isError = false;
    if (isIfaceAvailable(CK_SERVICE, CK_PATH, CK_IFACE)) {
        if (!doDbusAction(CK_SERVICE, CK_PATH, CK_IFACE)) {
            isError = true;
        }
    }
    else if (isIfaceAvailable(LD_SERVICE, LD_PATH, LD_IFACE)){
        if (!doDbusAction(LD_SERVICE, LD_PATH, LD_IFACE)) {
            isError = true;
        }
    }
    else {
        isError = true;
    }
    if (isError) {
        QMessageBox::critical(this,
                              tr("Errorr in DBUS"),
                              tr("Can't establish connection to\n"
                                 "org.freedesktop.ConsoleKit.Manager\n"
                                 "or to\n"
                                 "org.freedesktop.login1.Manager\n"
                                 "May be you have no permissions\n"
                                 "Or consolekit/logind app not available\n"
                                 "Or maybe you not in power group"));

    }
#endif
#ifdef Q_OS_WIN
    if (!doShutdown()) {
        doProcessRun();
    }
    onExit();
}

void MainWindow::doProcessRun()
{
    const QString method((isReboot_) ? "/r" : "/s");
    QProcess process;
    process.startDetached("shutdown", QStringList() << method);
}

bool MainWindow::doShutdown()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    LUID luid;
    uint flags = (isReboot_)? EWX_REBOOT : EWX_SHUTDOWN;
    DWORD reason = SHTDN_REASON_MINOR_MAINTENANCE
            | SHTDN_REASON_FLAG_PLANNED;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES
                          | TOKEN_QUERY, &hToken)) {
        return false;
    }
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken,
                          false,
                          &tkp,
                          0,
                          (PTOKEN_PRIVILEGES)NULL,
                          0);
    DWORD error = GetLastError();
    if (error == ERROR_NOT_ALL_ASSIGNED
            || error != ERROR_SUCCESS) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't get privileges"));
        return false;
    }
    bool result = ExitWindowsEx(flags, reason);
#ifdef IS_DEBUG
    qDebug() << result;
    qDebug() << GetLastError();
#endif
    tkp.Privileges[0].Attributes = 0;
    AdjustTokenPrivileges(hToken,
                          false,
                          &tkp,
                          0,
                          (PTOKEN_PRIVILEGES)NULL,
                          0);
    CloseHandle(hToken);
    return result;
#endif
}

#ifdef HAVE_DBUS
bool MainWindow::isIfaceAvailable(const QString &service, const QString &path, const QString &iface)
{
    QString checkMethod;
    if (service == CK_SERVICE) {
        checkMethod = (isReboot_) ? "Can"+CK_REBOOT : "Can"+CK_POWEROFF;
    }
    else {
        checkMethod = (isReboot_) ? "Can"+LD_REBOOT : "Can"+LD_POWEROFF;
    }
    QDBusInterface interface(service, path, iface, QDBusConnection::systemBus());
    if(interface.isValid()) {
#ifdef IS_DEBUG
        qDebug() << "Interface" << interface.interface() << "isValid";
        qDebug() << "checkMethod" << checkMethod;
#endif
        if (service == CK_SERVICE) {
            QDBusReply<bool> reply = interface.call(checkMethod.toLatin1());
#ifdef IS_DEBUG
            qDebug() << "reply" << reply;
#endif
            return reply;
        }
        if (service == LD_SERVICE) {
            QDBusReply<QString> reply = interface.call(checkMethod.toLatin1());
#ifdef IS_DEBUG
            qDebug() << "reply" << reply;
#endif
            return (reply == "yes");
        }
    }
    return false;
}

bool MainWindow::doDbusAction(const QString &service, const QString &path, const QString &iface)
{
    QString method;
    if (service == CK_SERVICE) {
        method = (isReboot_) ? CK_REBOOT : CK_POWEROFF;
    }
    else {
        method = (isReboot_) ? LD_REBOOT : LD_POWEROFF;
    }
    QDBusInterface interface(service, path, iface, QDBusConnection::systemBus());
    if(interface.isValid()) {
#ifdef IS_DEBUG
        qDebug() << "method" << method;
#endif
        QList<QVariant> args;
        args << QVariant(true);
        interface.callWithArgumentList(QDBus::NoBlock, method, args);
        onExit();
    }
    return false;
}
#endif
