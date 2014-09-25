/*
 * settingsdialog.h
 * Copyright (C) 2013-2014 Vitaly Tonkacheyev
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "defines.h"
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
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();
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

protected:
	void closeEvent(QCloseEvent *);

protected slots:
	void onSoundCard(int changed);
	void onMixer(const QString &changed);
	void onAutorun(bool toggle);
	void onOk();
	void onCancel();
	void onPBAction(QListWidgetItem *item);
	void onCPAction(QListWidgetItem *item);
	void onENAction(QListWidgetItem *item);
	void onDarkStyle(bool toggled);
	void onLightStyle(bool toggled);
	void onPulseSoundSystem(bool toggled);
signals:
	void soundCardChanged(int card);
	void mixerChanged(const QString &mixer);
	void autorunChanged(bool isIt);
	void playChanged(const QString &check, bool isIt);
	void captChanged(const QString &check, bool isIt);
	void enumChanged(const QString &check, bool isIt);
	void styleChanged(bool isLight);
	void soundSystemChanged(bool isAlsa);

private:
	Ui::SettingsDialog *ui;
	QStringList soundCards_;
	QStringList mixers_;
	bool isAutorun_;
	bool itemsAdded_;
	QListWidget *playbacks_;
	QListWidget *captures_;
	QListWidget *enums_;
	QLabel *l1_;
	QLabel *l2_;
	QLabel *l3_;
	const QString PLAYBACK_TIP = tr("Enable/Disable Alsa's Playback Switch");
	const QString CAPTURE_TIP = tr("Enable/Disable Alsa's Capture Switch");
	const QString ENUMS_TIP = tr("Enable/Disable Alsa's Enumerated Switch");
};

#endif // SETTINGSDIALOG_H
