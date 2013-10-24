/*
 * popupwindow.h
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

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QSystemTrayIcon>
#include <QDialog>


#include "alsawork/alsawork.h"
#include "settingsdialog.h"
#include "defines.h"

class QAction;
class QMenu;
class QSlider;
class QSettings;
class MixerSwitches;

class PopupWindow : public QDialog
{
	Q_OBJECT

public:
	PopupWindow();
	~PopupWindow();

protected:
	void closeEvent(QCloseEvent *);
	bool eventFilter(QObject *object, QEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

protected slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void showPopup();
	void showSettings();
	void onMute(bool isToggled);
	void onAbout();
	void onQuit();
	void onSlider(int value);
	void onMixerChanged(const QString &mixer);
	void onCardChanged(const QString &card);
	void onPlayback(const QString &name, bool isIt);
	void onCapture(const QString &name, bool isIt);
	void onEnum(const QString &name, bool isIt);
	void onAutorun(bool isIt);
	void onStyleChanged(bool isLight);

private:
	void setTrayIcon(int value);
	void createActions();
	void createTrayMenu();
	void setVolume(int value);
	void setIconToolTip(int value);
	void updateSwitches();
	void createDesktopFile();
	void readDesktopFile();

private:
	AlsaWork *alsaWork_;
	QAction *restore_;
	QAction *settings_;
	QAction *mute_;
	QAction *about_;
	QAction *exit_;
	QMenu *trayMenu_;
	QSystemTrayIcon *trayIcon_;
	QSlider *volumeSlider_;
	SettingsDialog *settingsDialog_;
	QString mixerName_;
	int cardIndex_;
	QString cardName_;
	QStringList cardList_;
	QStringList mixerList_;
	MixerSwitches switchList_;
	QList<switcher> playBackItems_;
	QList<switcher> captureItems_;
	QList<switcher> enumItems_;
	int volumeValue_;
	bool isMuted_;
	bool isAutorun_;
	bool isLightStyle_;
};

#endif // POPUPWINDOW_H
