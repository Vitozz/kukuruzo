/*
 * popupwindow.h
 * Copyright (C) 2013-2015 Vitaly Tonkacheyev
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
#include <QVBoxLayout>
#include <QTimer>

#include "alsawork/alsawork.h"
#ifdef USE_PULSE
#include "pulsework/pulsecore.h"
#endif
#include "settingsdialog.h"
#include "defines.h"

class QAction;
class QMenu;
class QSlider;
class QLabel;
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

protected slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void showPopup();
	void showSettings();
	void onMute(bool isToggled);
	void onAbout();
	inline void onQuit() {close();}
	void onSlider(int value);
	void onCardChanged(int card);
	void onAutorun(bool isIt);
	void onStyleChanged(bool isLight);
	void onMixerChanged(const QString &mixer);
	void onPlayback(const QString &name, bool isIt);
	void onCapture(const QString &name, bool isIt);
	void onEnum(const QString &name, bool isIt);
	void onSoundSystem(bool isIt);
	void onTimeout();
	void enablePolling(bool isIt);

private:
	void setTrayIcon(int value);
	void initActions();
	void updateTrayMenu();
	void setVolume(int value);
	void setIconToolTip(int value);
	void createDesktopFile();
	void readDesktopFile();
	QString getResPath(const QString &fileName) const;
	void updateSwitches();
	void setPopupPosition(const QPoint &point);

private:
	AlsaWork::Ptr alsaWork_;
#ifdef USE_PULSE
	PulseCore::Ptr pulse_;
	int deviceIndex_;
#endif
	QString mixerName_;
	int cardIndex_;
	QStringList mixerList_;
	MixerSwitches::Ptr switchList_;
	QList<switcher> playBackItems_;
	QList<switcher> captureItems_;
	QList<switcher> enumItems_;
	QAction *restore_;
	QAction *settings_;
	QAction *mute_;
	QAction *about_;
	QAction *aboutQt_;
	QAction *exit_;
	QMenu *trayMenu_;
	QSystemTrayIcon *trayIcon_;
	QVBoxLayout *mainLayout_;
	QSlider *volumeSlider_;
	QLabel *volumeLabel_;
	QTimer *pollingTimer_;
	SettingsDialog *settingsDialog_;
	QString cardName_;
	QStringList cardList_;
	QString pulseCardName_;
	QStringList pulseCardList_;
	int volumeValue_;
	int pollingVolume_;
	bool isMuted_;
	bool isAutorun_;
	bool isLightStyle_;
	bool isPulse_;
	bool isPoll_;
	QString title_;
	QString message_;
};

#endif // POPUPWINDOW_H
