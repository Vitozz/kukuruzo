/*
 * settingsdialog.h
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "../defines.h"
#include <QDialog>
#include <QLabel>

class QListWidget;
class QListWidgetItem;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;
    void setSoundCards(const QStringList &cards);
    void setMixers(const QStringList &mixers);
    void setAutorun(bool isAutorun);
    void connectSignals();
    void setCurrentCard(int index);
    void setCurrentMixer(const QString &mixer);
    void setPlaybackChecks(const QList<switcher> &pbList);
    void setCaptureChecks(const QList<switcher> &cList);
    void setEnumChecks(const QList<switcher> &eList);
    void setIconStyle(bool isLight);
    void setUsePulse(bool isPulse);
    void hideAlsaElements(bool isHide);
    void setPulseAvailable(bool available);
    void setUsePolling(bool isPoll);

protected:
    void closeEvent(QCloseEvent *) override;

protected slots:
    void onOk();
    void onPBAction(QListWidgetItem *item);
    void onCPAction(QListWidgetItem *item);
    void onENAction(QListWidgetItem *item);
    void onDarkStyle(bool toggled);
    void onLightStyle(bool toggled);
    void onPulseSoundSystem(bool toggled);
    void onEnableTimer(bool toggled);
signals:
    void soundCardChanged(int card);
    void mixerChanged(const QString &mixer);
    void autorunChanged(bool isIt);
    void playChanged(const QString &check, bool isIt);
    void captChanged(const QString &check, bool isIt);
    void enumChanged(const QString &check, bool isIt);
    void styleChanged(bool isLight);
    void soundSystemChanged(bool isAlsa);
    void timerEnabled(bool isTimer);

private:
    void disconnectSignals();

private:
    Ui::SettingsDialog *ui;
    QStringList soundCards_;
    QStringList mixers_;
    bool isAutorun_;
    bool pulseAvailable_;
    QListWidget *playbacks_;
    QListWidget *captures_;
    QListWidget *enums_;
    QLabel *l1_;
    QLabel *l2_;
    QLabel *l3_;
};

#endif // SETTINGSDIALOG_H
