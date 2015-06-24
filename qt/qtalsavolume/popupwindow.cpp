/*
 * popupwindow.cpp
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

#include "popupwindow.h"

#include <QtGui>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QDesktopWidget>

#ifdef ISDEBUG
#include <QDebug>
#endif

static const QString appLogo(":/images/Logo");
static const QString autoStartPath(".config/autostart");
static const QString fName = QDir::home().absolutePath() + "/.config/autostart/qtalsavolume.desktop";
static const QString dFile = QString("%1%2%3%4%5%6%7%8").arg(
			     "[Desktop Entry]\n",
			     "Encoding=UTF-8\n",
			     "Name=QtAlsaVolume\n",
			     "Exec=qtalsavolume\n",
			     QString("%1%2%3").arg("Version=",APP_VERSION,"\n"),
			     "Type=Application\n",
			     "Comment=Changes the sound volume from the system tray\n",
			     "X-LXQt-Need-Tray=true");
static const int POPUP_HEIGHT = 140;
static const int POPUP_WIDTH = 30;
static const int DELTA = 2;

PopupWindow::PopupWindow()
: alsaWork_(AlsaWork::Ptr(new AlsaWork())),
#ifdef USE_PULSE
  pulse_(PulseCore::Ptr(new PulseCore(APP_NAME))),
  deviceIndex_(0),
#endif
  mixerName_(QString()),
  cardIndex_(0),
  mixerList_(QStringList()),
  switchList_(MixerSwitches::Ptr()),
  playBackItems_(QList<switcher>()),
  captureItems_(QList<switcher>()),
  enumItems_(QList<switcher>()),
  restore_(new QAction(tr("&Restore"), this)),
  settings_(new QAction(tr("&Settings"), this)),
  mute_(new QAction(tr("&Mute"), this)),
  about_(new QAction(tr("&About..."), this)),
  aboutQt_(new QAction(tr("About Qt"), this)),
  exit_(new QAction(tr("&Quit"), this)),
  trayMenu_(new QMenu(this)),
  trayIcon_(new QSystemTrayIcon(this)),
  mainLayout_(new QVBoxLayout()),
  volumeSlider_(new QSlider(Qt::Vertical, this)),
  volumeLabel_(new QLabel(this)),
  pollingTimer_(new QTimer(this)),
  settingsDialog_(new SettingsDialog(this)),
  cardName_(QString()),
  cardList_(QStringList()),
  pulseCardName_(QString()),
  pulseCardList_(QStringList()),
  volumeValue_(0),
  pollingVolume_(0),
  isMuted_(false),
  isAutorun_(false),
  isLightStyle_(false),
  isPulse_(false),
  isPoll_(true),
  title_(tr("About QtAlsaVolume")),
#ifdef USE_PULSE
  message_(QString(tr("<!DOCTYPE html><html><body>"
		      "<p><b>Tray Alsa Volume Changer written using Qt</b></p>"
		      "<p>With Pulseaudio support</p>"
		      "<p>2015 (c) Vitaly Tonkacheyev <address><a href=\"mailto:thetvg@gmail.com\">&lt;EMail&gt;</a></address></p>"
		      "<a href=\"http://sites.google.com/site/thesomeprojects/\">Program WebSite</a>"
		      "<p>version: <b>%1</b></p></body></html>")).arg(APP_VERSION))
#else
  message_(QString(tr("<!DOCTYPE html><html><body>"
		      "<p><b>Tray Alsa Volume Changer written using Qt</b></p>"
		      "<p>2015 (c) Vitaly Tonkacheyev <address><a href=\"mailto:thetvg@gmail.com\">&lt;EMail&gt;</a></address></p>"
		      "<a href=\"http://sites.google.com/site/thesomeprojects/\">Program WebSite</a>"
		      "<p>version: <b>%1</b></p></body></html>")).arg(APP_VERSION))
#endif
{
	setWindowIcon(QIcon(appLogo));
	//Start of tray icon initialization
	initActions();
	updateTrayMenu();
	const QString errorHeader(tr("Error"));
	const QString systrayMissing(tr("System tray is not available"));
	if(!trayIcon_->isSystemTrayAvailable()) {
		QMessageBox::critical(this, errorHeader, systrayMissing);
	}
	trayIcon_->setContextMenu(trayMenu_);
	//Adding QLabel and QSlider to PopupWindow
	volumeSlider_->setRange(0,100);
	volumeSlider_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	volumeLabel_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	QFont font = volumeLabel_->font();
	font.setBold(true);
	volumeLabel_->setFont(font);
	mainLayout_->addWidget(volumeLabel_);
	mainLayout_->addWidget(volumeSlider_);
	mainLayout_->setContentsMargins(0,4,0,4);
	mainLayout_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setLayout(mainLayout_);
	setMinimumHeight(POPUP_HEIGHT);
	setMinimumWidth(POPUP_WIDTH);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//Reading settings and alsa variables
	QSettings setts_;
	isLightStyle_ = setts_.value(ICOSTYLE, true).toBool();
	isAutorun_ = setts_.value(ISAUTO, false).toBool();
	isPoll_ = setts_.value(ISPOLL, true).toBool();
#ifdef USE_PULSE
	const QString pulseIsMissing(tr("Can't start PulseAudio. Using Alsa by default"));
	isPulse_ = setts_.value(PULSE, false).toBool();
	if (pulse_->available()) {
		pulseCardList_ = pulse_->getCardList();
		const QString lastSink = setts_.value(LAST_SINK, "").toString();
		pulseCardName_ = (!lastSink.isEmpty()) ? lastSink : pulse_->defaultSink();
		pulse_->setCurrentDevice(pulseCardName_);
		deviceIndex_ = pulse_->getCurrentDeviceIndex();
		if (isPulse_) {
			isMuted_ = pulse_->getMute();
			volumeValue_ = pulse_->getVolume();
		}
	}
	else {
		QMessageBox::critical(this, errorHeader, pulseIsMissing);
		pulse_.clear();
		isPulse_ = false;
		settingsDialog_->setPulseAvailable(false);
	}
#else
	isPulse_ = false;
	settingsDialog_->setPulseAvailable(false);
#endif
	cardList_ = alsaWork_->getCardsList();
	cardIndex_ = setts_.value(CARD_INDEX, 0).toInt();
	if (alsaWork_->cardExists(cardIndex_)) {
		alsaWork_->setCurrentCard(cardIndex_);
		if (!alsaWork_->haveVolumeMixers()) {
			cardIndex_ = alsaWork_->getFirstCardWithMixers();
			alsaWork_->setCurrentCard(cardIndex_);
		}
	}
	mixerList_ = alsaWork_->getVolumeMixers();
	QString mixer = mixerList_.contains("Master") ? "Master" : mixerList_.at(0);
	mixerName_ = setts_.value(MIXER_NAME, mixer).toString();
	cardName_ = alsaWork_->getCardName(cardIndex_);
	if (!isPulse_) {
		volumeValue_ = alsaWork_->getAlsaVolume();
		isMuted_ = alsaWork_->getMute();
	}
	updateSwitches();
	if (!isPulse_) {
		settingsDialog_->setSoundCards(cardList_);
		settingsDialog_->setCurrentCard(cardIndex_);
	}
#ifdef USE_PULSE
	else if (pulse_) {
		settingsDialog_->setSoundCards(pulseCardList_);
		settingsDialog_->setCurrentCard(pulse_->getCurrentDeviceIndex());
		settingsDialog_->setUsePulse(isPulse_);
		settingsDialog_->hideAlsaElements(isPulse_);
	}
#endif
	settingsDialog_->setIconStyle(isLightStyle_);
	settingsDialog_->connectSignals(); //connecting settingsDialog_ internal signals
	connect(settingsDialog_, SIGNAL(soundCardChanged(int)), this, SLOT(onCardChanged(int)));
	connect(settingsDialog_, SIGNAL(mixerChanged(QString)), this, SLOT(onMixerChanged(QString)));
	connect(settingsDialog_, SIGNAL(playChanged(QString,bool)), this, SLOT(onPlayback(QString,bool)));
	connect(settingsDialog_, SIGNAL(captChanged(QString,bool)), this, SLOT(onCapture(QString,bool)));
	connect(settingsDialog_, SIGNAL(enumChanged(QString,bool)), this, SLOT(onEnum(QString,bool)));
	connect(settingsDialog_, SIGNAL(autorunChanged(bool)), this, SLOT(onAutorun(bool)));
	connect(settingsDialog_, SIGNAL(styleChanged(bool)), this, SLOT(onStyleChanged(bool)));
	connect(settingsDialog_, SIGNAL(timerEnabled(bool)), this, SLOT(enablePolling(bool)));
#ifdef USE_PULSE
	if (pulse_) {
		connect(settingsDialog_, SIGNAL(soundSystemChanged(bool)), this, SLOT(onSoundSystem(bool)));
	}
#endif
	createDesktopFile();
	//Finish of tray icon initialization
	mute_->setChecked(isMuted_);
	pollingVolume_ = volumeValue_;
	volumeSlider_->setValue(volumeValue_);
	volumeLabel_->setText(QString::number(volumeValue_));
	setIconToolTip(volumeValue_);
	setTrayIcon(volumeValue_);
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(volumeSlider_, SIGNAL(valueChanged(int)), this, SLOT(onSlider(int)));
	trayIcon_->installEventFilter(this);
	installEventFilter(this);
	trayIcon_->show();
	pollingTimer_->setInterval(POLLING_INTERVAL);
	connect(pollingTimer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
	pollingTimer_->start();
}

PopupWindow::~PopupWindow()
{

	delete settingsDialog_;
	if (pollingTimer_->isActive()) {
		pollingTimer_->stop();
	}
	delete pollingTimer_;
	delete volumeLabel_;
	delete volumeSlider_;
	delete mainLayout_;
	delete trayIcon_;
	delete trayMenu_;
	delete exit_;
	delete aboutQt_;
	delete about_;
	delete mute_;
	delete settings_;
	delete restore_;
}

void PopupWindow::initActions()
{
	connect(restore_, SIGNAL(triggered()), this, SLOT(showPopup()));
	connect(settings_, SIGNAL(triggered()), this, SLOT(showSettings()));
	mute_->setCheckable(true);
	connect(mute_, SIGNAL(toggled(bool)), this, SLOT(onMute(bool)));
	connect(about_, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(aboutQt_, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(exit_, SIGNAL(triggered()), this, SLOT(onQuit()));
}

void PopupWindow::updateTrayMenu()
{
	trayMenu_->addAction(restore_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(settings_);
	trayMenu_->addAction(mute_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(about_);
	trayMenu_->addAction(aboutQt_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(exit_);
}

void PopupWindow::onAbout()
{
	QMessageBox::about(this, title_, message_);
}

void PopupWindow::showPopup()
{
	if (!this->isVisible() && trayIcon_->isVisible()) {
		this->show();
		setPopupPosition(QCursor::pos());
	}
	else {
		this->hide();
	}
}

void PopupWindow::setTrayIcon(int value)
{
	const QString pathPrefix = (isLightStyle_) ? "icons/light/" : "icons/dark/";
	int number = 100;
	value = (value <= 0) ? 0 : (value > 100) ? 100 : value;
	number = (value < 10) ? 10 : int(value/10)*10;
	if (value <= 0) {
		number = 0;
	}
	if (isMuted_) {
		number = 0;
	}
	QString pathSuffix = QString("tb_icon%1.png").arg(QString::number(number));
	const QString fullPath = getResPath(QString("%1%2").arg(pathPrefix,pathSuffix));
#ifdef ISDEBUG
	qDebug() << "val " << value;
	qDebug() << "num " << number;
	qDebug() << "Suffix " << pathSuffix;
	qDebug() << "Icon path " << fullPath;
#endif
	trayIcon_->setIcon(QIcon(fullPath));
}

void PopupWindow::showSettings()
{
	settingsDialog_->setIconStyle(isLightStyle_);
	settingsDialog_->setUsePolling(isPoll_);
#ifdef USE_PULSE
	if (isPulse_ && pulse_) {
		settingsDialog_->blockSignals(true);
		settingsDialog_->setSoundCards(pulseCardList_);
		settingsDialog_->blockSignals(false);
		settingsDialog_->setCurrentCard(pulse_->getCurrentDeviceIndex());
	}
#endif
	if (!isPulse_) {
		settingsDialog_->setCurrentCard(cardIndex_);
	}
	settingsDialog_->setMixers(mixerList_);
	settingsDialog_->setCurrentMixer(mixerName_);
	updateSwitches();
	settingsDialog_->setPlaybackChecks(playBackItems_);
	settingsDialog_->setCaptureChecks(captureItems_);
	settingsDialog_->setEnumChecks(enumItems_);
	readDesktopFile();
	settingsDialog_->showNormal();
}

void PopupWindow::onMute(bool isToggled)
{
	isMuted_ = isToggled;
#ifdef USE_PULSE
	if (isPulse_ && pulse_) {
		pulse_->setMute(isToggled);
	}
#endif
	if (!isPulse_) {
		alsaWork_->setMute(!isToggled);
	}
	setTrayIcon(volumeValue_);
}

void PopupWindow::closeEvent(QCloseEvent *)
{
	QSettings setts_;
#ifdef USE_PULSE
	if (pulse_) {
		setts_.setValue(LAST_SINK, pulseCardName_);
		setts_.setValue(PULSE, isPulse_);
	}
#endif
	setts_.setValue(CARD_INDEX, cardIndex_);
	setts_.setValue(MIXER_NAME, mixerName_);
	setts_.setValue(ISAUTO, isAutorun_);
	setts_.setValue(ICOSTYLE, isLightStyle_);
	setts_.setValue(ISPOLL, isPoll_);
	qApp->quit();
}

void PopupWindow::setPopupPosition(const QPoint &point)
{
	const QRect trayGeometry = trayIcon_->geometry();
	QPoint to;
	Position position;
	QDesktopWidget desktopWidget;
	const int screenHeight = desktopWidget.availableGeometry(this).height();
	const int screenTop = desktopWidget.availableGeometry(this).top();
	if (!trayGeometry.isEmpty()) {
		position = trayGeometry.top() > screenHeight/2 ? BOTTOM : TOP;
		to.setX(trayGeometry.left() + trayGeometry.width()/2 - width()/2);
	}
	else {
		position = (point.y() > screenHeight/2) ? BOTTOM : TOP;
		to.setX(point.x() - width()/2);
	}
	switch (position) {
	case TOP:
		to.setY(screenTop + 2);
		break;
	case BOTTOM:
		to.setY(screenHeight - height() - 2);
		break;
	default:
		break;
	}
	this->setGeometry(to.x(), to.y(), width(), height());
}

bool PopupWindow::eventFilter(QObject *object, QEvent *event)
{
	if (object == trayIcon_) {
		if (event->type() == QEvent::Wheel) {
			QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
#ifdef ISDEBUG
			qDebug() << "WheelEvent:Delta " << wheelEvent->delta();
#endif
			const int steps = (wheelEvent->delta()>0) ? 1 : (wheelEvent->delta()<0) ? -1: 0;
			setVolume(steps);
			return true;
		}
	}
	if (object == this) {
		if (event->type() == QEvent::Leave || event->type() == QEvent::WindowDeactivate) {
#ifdef ISDEBUG
			qDebug() << "Mouse Leave";
#endif
			this->hide();
			return true;
		}
	}
	return false;
}

void PopupWindow::setVolume(int value)
{
	volumeValue_ += value*DELTA_VOLUME;
	volumeValue_ = (volumeValue_ <= 0) ? 0 : (volumeValue_ > 100) ? 100 : volumeValue_;
	volumeSlider_->setValue(volumeValue_);
}

void PopupWindow::onSlider(int value)
{
	volumeValue_ = value;
	pollingVolume_ = value;
#ifdef USE_PULSE
	if (isPulse_ && pulse_) {
		pulse_->setVolume(volumeValue_);
	}
#endif
	if (!isPulse_) {
		if (alsaWork_->haveVolumeMixers()) {
			alsaWork_->setAlsaVolume(volumeValue_);
			pollingVolume_ = alsaWork_->getAlsaVolume();
		}
	}
	volumeLabel_->setText(QString::number(volumeValue_));
	setTrayIcon(volumeValue_);
	setIconToolTip(volumeValue_);
}

void PopupWindow::setIconToolTip(int value)
{
#ifdef USE_PULSE
	if (isPulse_) {
		if (pulse_) {
			const QString message = tr("Card: %1%2%3").arg(
									pulse_->getDeviceDescription(pulseCardName_),
									"\n",
									tr("Volume: %1").arg(QString::number(value))
						);
			trayIcon_->setToolTip(message);
		}
	}
#endif
	if (!isPulse_) {
		const QString message = tr("Card: %1%2%3%4").arg(
								 cardName_,
								 "\n",
								 tr("Mixer: %1%2").arg(mixerName_,"\n"),
								 tr("Volume: ").arg(QString::number(value))
					);
		trayIcon_->setToolTip(message);
	}
}

void PopupWindow::onCardChanged(int card)
{
#ifdef USE_PULSE
	if (isPulse_ && pulse_) {
		const QString oldCard = pulseCardName_;
		pulseCardName_ = pulse_->getDeviceNameByIndex(card);
		pulse_->setCurrentDevice(pulseCardName_);
		pulseCardList_ = pulse_->getCardList();
		settingsDialog_->blockSignals(true); //Block Signals
		settingsDialog_->setSoundCards(pulseCardList_);
		settingsDialog_->setCurrentCard(pulse_->getCurrentDeviceIndex());
		settingsDialog_->blockSignals(false); //Block Signals
		deviceIndex_ = pulse_->getCurrentDeviceIndex();
		volumeValue_ = pulse_->getVolume();
		volumeSlider_->setValue(volumeValue_);
		if (oldCard != pulseCardName_) {
			updateAlsa(pulse_->getCardIndex());
		}
	}
#endif
	if (!isPulse_) {
		updateAlsa(card);
	}
}

void PopupWindow::onMixerChanged(const QString &mixer)
{
	if (mixerList_.contains(mixer) && !isPulse_) {
		mixerName_ = mixer;
		alsaWork_->setCurrentMixer(mixerList_.indexOf(mixerName_));
		volumeValue_ = alsaWork_->getAlsaVolume();
		volumeSlider_->setValue(volumeValue_);
	}
}

void PopupWindow::updateAlsa(int card)
{
	cardIndex_ = alsaWork_->cardExists(card) ? card : 0;
	cardName_ = alsaWork_->getCardName(cardIndex_);
	alsaWork_->setCurrentCard(cardIndex_);
	mixerList_ = alsaWork_->getVolumeMixers();
	settingsDialog_->setMixers(mixerList_);
	updateSwitches();
	settingsDialog_->setPlaybackChecks(playBackItems_);
	settingsDialog_->setCaptureChecks(captureItems_);
	settingsDialog_->setEnumChecks(enumItems_);
}

void PopupWindow::updateSwitches()
{
	switchList_ = alsaWork_->getSwitchList();
	if (switchList_ && !switchList_->isEmpty()) {
		playBackItems_ = switchList_->playbackSwitchList();
		captureItems_ = switchList_->captureSwitchList();
		enumItems_ = switchList_->enumSwitchList();
	}
}

void PopupWindow::onPlayback(const QString &name, bool isIt)
{
	alsaWork_->setSwitch(name, PLAYBACK, isIt);
}

void PopupWindow::onCapture(const QString &name, bool isIt)
{
	alsaWork_->setSwitch(name, CAPTURE ,isIt);
}

void PopupWindow::onEnum(const QString &name, bool isIt)
{
	alsaWork_->setSwitch(name, ENUM, isIt);
}

void PopupWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		showPopup();
		break;
	case QSystemTrayIcon::MiddleClick:
		mute_->setChecked(!mute_->isChecked());
		break;
	default:
	;
	}
}

void PopupWindow::onAutorun(bool isIt)
{
	if (isAutorun_ != isIt) {
		isAutorun_ = isIt;
		QSettings setts;
		setts.setValue(ISAUTO, isAutorun_);
		setts.sync();
		createDesktopFile();
	}
}

void PopupWindow::createDesktopFile()
{
	QDir home = QDir::home();
	if (!home.exists(autoStartPath)) {
		home.mkpath(autoStartPath);
	}
	QFile f(fName);
	if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		f.write(dFile.trimmed().toUtf8());
		f.write(QString("\nHidden=%1").arg(isAutorun_ ? "false\n" : "true\n").toUtf8());
	}
}

void PopupWindow::readDesktopFile()
{
	QFile desktop(fName);
	if (desktop.open(QIODevice::ReadOnly))
	{
		settingsDialog_->setAutorun(QString(desktop.readAll()).contains(QRegExp("\\bhidden\\s*=\\s*false", Qt::CaseInsensitive)));
	}
}

void PopupWindow::onStyleChanged(bool isLight)
{
	isLightStyle_ = isLight;
	setTrayIcon(volumeValue_);
}

QString PopupWindow::getResPath(const QString &fileName) const
{
	const QStringList resDirs = QStringList()
				    << QString(QDir::currentPath())
				    << QString(qApp->applicationDirPath())
				    << QString("/usr/share/%1").arg(APP_NAME)
				    << QString("/usr/local/share/%1").arg(APP_NAME)
				    << QString(QDir::home().absolutePath() + "/.local/share/%1").arg(APP_NAME)
				    << QString(QDir::currentPath().left(QDir::currentPath().lastIndexOf("/")) + "/share/%1").arg(APP_NAME);
	foreach(const QString &dir, resDirs){
		const QString fullName = QString("%1%2%3").arg(dir,"/",fileName);
		if (QFile::exists(fullName)) {
			return fullName;
		}
	}
	return QString();
}

void PopupWindow::onTimeout()
{
	if(isPoll_) {
		if (!isPulse_) {
			if (alsaWork_->haveVolumeMixers()) {
				const int volume = alsaWork_->getAlsaVolume();
				bool ismute = alsaWork_->getMute();
				if (pollingVolume_ != volume) {
					pollingVolume_ = volume;
					volumeSlider_->setValue(pollingVolume_);
				}
				if (isMuted_ != ismute) {
					isMuted_ = ismute;
					mute_->setChecked(isMuted_);
				}
			}
		}
#ifdef USE_PULSE
		if (isPulse_ && pulse_) {
			const int volume = pulse_->getVolume();
			bool ismute = pulse_->getMute();
			if (pollingVolume_ != volume) {
				pollingVolume_ = volume;
				volumeSlider_->setValue(pollingVolume_);
			}
			if (isMuted_ != ismute) {
				isMuted_ = ismute;
				mute_->setChecked(isMuted_);
			}
		}
#endif
	}
}

void PopupWindow::onSoundSystem(bool isIt)
{
	isPulse_ = isIt;
	if (isPulse_) {
#ifdef USE_PULSE
		if (pulse_) {
			pulseCardList_ = pulse_->getCardList();
			settingsDialog_->hideAlsaElements(isPulse_);
			settingsDialog_->setSoundCards(pulseCardList_);
			settingsDialog_->setCurrentCard(deviceIndex_);
		}
#endif
	}
	else {
		settingsDialog_->hideAlsaElements(isPulse_);
		settingsDialog_->setSoundCards(cardList_);
		settingsDialog_->setCurrentCard(cardIndex_);
	}
}

void PopupWindow::enablePolling(bool isIt)
{
	isPoll_ = isIt;
}
