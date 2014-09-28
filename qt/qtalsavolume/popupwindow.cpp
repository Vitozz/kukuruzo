/*
 * popupwindow.cpp
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

#include "popupwindow.h"

#include <QtGui>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#ifdef HAVE_QT5
#include <QScreen>
#endif
#ifdef ISDEBUG
#include <QDebug>
#endif

static const QStringList icoPrefix = QStringList()
				     << "tb_icon0.png" //0
				     << "tb_icon20.png" //1
				     << "tb_icon40.png" //2
				     << "tb_icon60.png" //3
				     << "tb_icon80.png" //4
				     << "tb_icon100.png"; //5
static const QString autoStartPath = ".config/autostart";
static const QString fName = QDir::home().absolutePath() + "/.config/autostart/qtalsavolume.desktop";
static const QString dFile = "[Desktop Entry]\n"
			     "Encoding=UTF-8\n"
			     "Name=QtAlsaVolume\n"
			     "Exec=qtalsavolume\n"
			     "Version=" +
			     qApp->applicationVersion() +
			     "\n"
			     "Type=Application\n"
			     "Comment=Changes the volume of ALSA from the system tray\n";
static const int POPUP_HEIGHT = 140;
static const int POPUP_WIDTH = 30;
static const int DELTA = 2;

PopupWindow::PopupWindow()
: alsaWork_(new AlsaWork),
#ifdef USE_PULSE
  pulse_(new PulseCore(APP_NAME)),
  deviceIndex_(0),
#endif
  mixerName_(QString()),
  cardIndex_(0),
  mixerList_(QStringList()),
  switchList_(new MixerSwitches()),
  playBackItems_(QList<switcher>()),
  captureItems_(QList<switcher>()),
  enumItems_(QList<switcher>()),
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
  settingsDialog_(new SettingsDialog(this)),
  cardName_(QString()),
  cardList_(QStringList()),
  pulseCardName_(QString()),
  pulseCardList_(QStringList()),
  volumeValue_(0),
  isMuted_(false),
  isAutorun_(false),
  isLightStyle_(false),
  isPulse_(false),
  title_(tr("About QtAlsaVolume")),
#ifdef USE_PULSE
  message_(QString(tr("Tray Alsa Volume Changer written using Qt\n\nWith Pulseaudio support\n\n2014 (c) Vitaly Tonkacheyev (thetvg@gmail.com)\n\nversion: %1")).arg(APP_VERSION))
#else
  message_(QString(tr("Tray Alsa Volume Changer written using Qt\n\n2014 (c) Vitaly Tonkacheyev (thetvg@gmail.com)\n\nversion: %1")).arg(APP_VERSION))
#endif
{
	setWindowIcon(QIcon(":/images/icons/volume_ico.png"));
	//Start of tray icon initialization
	initActions();
	updateTrayMenu();
	if(!trayIcon_->isSystemTrayAvailable()) {
		QMessageBox::critical(this, "Error", "System tray is not available");
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
	setWindowFlags(Qt::FramelessWindowHint);
	this->setMouseTracking(true);
	//Reading settings and alsa variables
	QSettings setts_;
	isLightStyle_ = setts_.value(ICOSTYLE, true).toBool();
	isAutorun_ = setts_.value(ISAUTO, false).toBool();
#ifdef USE_PULSE
	isPulse_ = setts_.value(PULSE, false).toBool();
	pulseCardList_ = pulse_->getCardList();
	QString lastSink = setts_.value(LAST_SINK, "").toString();
	if (!lastSink.isEmpty()) {
		pulseCardName_ = lastSink;
	}
	else {
		pulseCardName_ = pulse_->defaultSink();
	}
	pulse_->setCurrentDevice(pulseCardName_);
	deviceIndex_ = pulse_->getCurrentDeviceIndex();
	if (isPulse_) {
		isMuted_ = pulse_->getMute();
		volumeValue_ = pulse_->getVolume();
	}
#else
	isPulse_ = false;
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
		isMuted_ = !alsaWork_->getMute();
	}
	switchList_ = new MixerSwitches(alsaWork_->getSwitchList());
	updateSwitches();
	if (!isPulse_) {
		settingsDialog_->setSoundCards(cardList_);
		settingsDialog_->setCurrentCard(cardIndex_);
	}
#ifdef USE_PULSE
	else {
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
	connect(settingsDialog_,SIGNAL(styleChanged(bool)), this, SLOT(onStyleChanged(bool)));
#ifdef USE_PULSE
	connect(settingsDialog_, SIGNAL(soundSystemChanged(bool)), this, SLOT(onSoundSystem(bool)));
#endif
	createDesktopFile();
	//Finish of tray icon initialization
	mute_->setChecked(isMuted_);
	volumeSlider_->setValue(volumeValue_);
	volumeLabel_->setText(QString::number(volumeValue_));
	setIconToolTip(volumeValue_);
	setTrayIcon(volumeValue_);
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(volumeSlider_, SIGNAL(valueChanged(int)), this, SLOT(onSlider(int)));
	trayIcon_->installEventFilter(this);
	trayIcon_->show();
}

PopupWindow::~PopupWindow()
{

	delete settingsDialog_;
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
	delete switchList_;
#ifdef USE_PULSE
	delete pulse_;
#endif
	delete alsaWork_;
}

void PopupWindow::initActions()
{
	connect(settings_, SIGNAL(triggered()), this, SLOT(showSettings()));
	mute_->setCheckable(true);
	connect(mute_, SIGNAL(toggled(bool)), this, SLOT(onMute(bool)));
	connect(about_, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(aboutQt_, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(exit_, SIGNAL(triggered()), this, SLOT(onQuit()));
}

void PopupWindow::updateTrayMenu()
{
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
		setPopupPosition(QCursor::pos());
		this->show();
	}
	else {
		this->hide();
	}
}

void PopupWindow::setTrayIcon(int value)
{
	const QString pathPrefix = (isLightStyle_) ? "icons/light/" : "icons/dark/";
	QString pathSuffix = icoPrefix.at(5);
	if (value <= 0) {
		pathSuffix = icoPrefix.at(0);
	}
	if (value >0 && (value < 40)) {
		pathSuffix = icoPrefix.at(1);
	}
	if (value >=40 && (value < 60)) {
		pathSuffix = icoPrefix.at(2);
	}
	if (value >=60 && (value < 80)) {
		pathSuffix = icoPrefix.at(3);
	}
	if (value >=80 && (value < 100)) {
		pathSuffix = icoPrefix.at(4);
	}
	if (value >= 100) {
		pathSuffix = icoPrefix.at(5);
	}
	if (isMuted_) {
		pathSuffix = icoPrefix.at(0);
	}
	const QString fullPath = getResPath(pathPrefix + pathSuffix);
	trayIcon_->setIcon(QIcon(fullPath));
}

void PopupWindow::showSettings()
{
	settingsDialog_->setIconStyle(isLightStyle_);
#ifdef USE_PULSE
	if (isPulse_) {
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
	if (isPulse_) {
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
	setts_.setValue(LAST_SINK, pulseCardName_);
	setts_.setValue(PULSE, isPulse_);
#endif
	setts_.setValue(CARD_INDEX, cardIndex_);
	setts_.setValue(MIXER_NAME, mixerName_);
	setts_.setValue(ISAUTO, isAutorun_);
	setts_.setValue(ICOSTYLE, isLightStyle_);
	qApp->quit();
}

void PopupWindow::setPopupPosition(const QPoint &point)
{
	const QRect trayGeometry = trayIcon_->geometry();
	QPoint to;
	Position position;
#ifdef HAVE_QT5
	const int screenHeight = qApp->primaryScreen()->availableGeometry().height();
	const int screenTop = qApp->primaryScreen()->availableGeometry().top();
#else
	const int screenHeight = qApp->desktop()->availableGeometry().height();
	const int screenTop = qApp->desktop()->availableGeometry().top();
#endif
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
			const int degs = wheelEvent->delta()/8;
			const int steps = degs/15;
			setVolume(steps);
			return true;
		}
	}
	return false;
}

void PopupWindow::mouseMoveEvent(QMouseEvent *event)
{
	const int x = event->x();
	const int y = event->y();
	const int w = this->geometry().width();
	if ( ((x - DELTA) <= 0 || (x + DELTA) >= w || (y - DELTA) <= 0 ) && this->isVisible()) {
		this->hide();
	}
}

void PopupWindow::setVolume(int value)
{
	volumeValue_ += value*2;
	if (volumeValue_ <= 0) {
		volumeValue_ = 0;
	}
	if (volumeValue_ >100) {
		volumeValue_ = 100;
	}
	volumeSlider_->setValue(volumeValue_);
}

void PopupWindow::onSlider(int value)
{
#ifdef USE_PULSE
	if (isPulse_) {
		pulse_->setVolume(value);
	}
#endif
	if (!isPulse_) {
		alsaWork_->setAlsaVolume(value);
	}
	volumeValue_ = value;
	volumeLabel_->setText(QString::number(value));
	setTrayIcon(value);
	setIconToolTip(value);
}

void PopupWindow::setIconToolTip(int value)
{
	if (isPulse_) {
#ifdef USE_PULSE
		const QString message = tr("Card: ")
					+ pulse_->getDeviceDescription(pulseCardName_)
					+ "\n"+ tr("Volume: ")
					+ QString::number(value);
		trayIcon_->setToolTip(message);
#endif
		//
	}
	else {
		const QString message = tr("Card: ")
					+ cardName_
					+ "\n"
					+ tr("Mixer: ")
					+ mixerName_
					+ "\n"
					+ tr("Volume: ")
					+ QString::number(value);
		trayIcon_->setToolTip(message);
	}
}

void PopupWindow::onCardChanged(int card)
{
#ifdef USE_PULSE
	if (isPulse_) {
		pulseCardName_ = pulse_->getDeviceNameByIndex(card);
		pulse_->setCurrentDevice(pulseCardName_);
		deviceIndex_ = pulse_->getCurrentDeviceIndex();
		volumeValue_ = pulse_->getVolume();
		volumeSlider_->setValue(volumeValue_);
	}
#endif
	if (!isPulse_) {
		cardIndex_ = alsaWork_->cardExists(card) ? card : 0;
		cardName_ = alsaWork_->getCardName(cardIndex_);
		alsaWork_->setCurrentCard(cardIndex_);
		mixerList_ = alsaWork_->getVolumeMixers();
		settingsDialog_->setMixers(mixerList_);
		switchList_ = new MixerSwitches(alsaWork_->getSwitchList());
		updateSwitches();
		settingsDialog_->setPlaybackChecks(playBackItems_);
		settingsDialog_->setCaptureChecks(captureItems_);
		settingsDialog_->setEnumChecks(enumItems_);
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

void PopupWindow::updateSwitches()
{
	if (!switchList_->isEmpty()) {
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
				    << QString("/usr/share/qtalsavolume")
				    << QString("/usr/local/share/qtalsavolume")
				    << QString(QDir::home().absolutePath() + "/.local/share/qtalsavolume");
	foreach(const QString &dir, resDirs){
		const QString fullName = dir + "/" + fileName;
		if (QFile::exists(fullName)) {
			return fullName;
		}
	}
	return QString();
}

void PopupWindow::onSoundSystem(bool isIt)
{
	isPulse_ = isIt;
	if (isPulse_) {
#ifdef USE_PULSE
		pulseCardList_ = pulse_->getCardList();
		settingsDialog_->hideAlsaElements(isPulse_);
		settingsDialog_->setSoundCards(pulseCardList_);
		settingsDialog_->setCurrentCard(deviceIndex_);
#endif
	}
	else {
		settingsDialog_->hideAlsaElements(isPulse_);
		settingsDialog_->setSoundCards(cardList_);
		settingsDialog_->setCurrentCard(cardIndex_);
	}
}
