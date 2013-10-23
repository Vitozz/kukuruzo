#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QSystemTrayIcon>
#include <QDialog>


#include "alsawork/alsawork.h"
#include "settingsdialog.h"

class QAction;
class QMenu;
class QSlider;
class QSettings;
class MixerSwitches;

typedef QPair<QString, bool> switcher;

enum Position {
	BOTTOM = 0,
	TOP = 1,
	LEFT = 2,
	RIGHT = 3
};

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

private:
	void setTrayIcon(int value);
	void createActions();
	void createTrayMenu();
	void setVolume(int value);
	void setIconToolTip(int value);
	void updateSwitches();

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
	QSettings *setts_;
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
	bool isAutoun_;
};

#endif // POPUPWINDOW_H
