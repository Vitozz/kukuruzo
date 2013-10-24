/*
 * settingsdialog.h
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "defines.h"
#include <QDialog>

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
	void setSoundCards(QStringList cards);
	void setMixers(QStringList mixers);
	void setAutorun(bool isAutorun);
	void connectSignals();
	void setCurrentCard(const QString &card);
	void setCurrentMixer(const QString &mixer);
	void setPlaybackChecks(QList<switcher> pbList);
	void setCaptureChecks(QList<switcher> cList);
	void setEnumChecks(QList<switcher> eList);
	void setIconStyle(bool isLight);

protected:
	void closeEvent(QCloseEvent *);

protected slots:
	void onSoundCard(const QString &changed);
	void onMixer(const QString &changed);
	void onAutorun(bool toggle);
	void onOk();
	void onCancel();
	void onPBAction(QListWidgetItem *item);
	void onCPAction(QListWidgetItem *item);
	void onENAction(QListWidgetItem *item);
	void onDarkStyle(bool toggled);
	void onLightStyle(bool toggled);
signals:
	void soundCardChanged(const QString &card);
	void mixerChanged(const QString &mixer);
	void autorunChanged(bool isIt);
	void playChanged(const QString &check, bool isIt);
	void captChanged(const QString &check, bool isIt);
	void enumChanged(const QString &check, bool isIt);
	void styleChanged(bool isLight);

private:
	Ui::SettingsDialog *ui;
	QStringList soundCards_;
	QStringList mixers_;
	bool isAutorun_;
	bool itemsAdded_;
	QListWidget *playbacks_;
	QListWidget *captures_;
	QListWidget *enums_;
};

#endif // SETTINGSDIALOG_H
