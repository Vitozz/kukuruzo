/*
 * popupwindow.cpp
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

#include "popupwindow.h"

#include <QtGui>
#include <QDesktopWidget>

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

PopupWindow::PopupWindow()
{
	setWindowIcon(QIcon(":/images/icons/volume_ico.png"));
	//Start of tray icon initialization
	createActions();
	createTrayMenu();
	trayIcon_ = new QSystemTrayIcon(this);
	trayIcon_->setContextMenu(trayMenu_);
	//Adding QLabel and QSlider to PopupWindow
	QVBoxLayout *mainLayout = new QVBoxLayout;
	volumeSlider_ = new QSlider(Qt::Vertical, this);
	volumeSlider_->setRange(0,100);
	volumeSlider_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	volumeLabel_ = new QLabel(this);
	volumeLabel_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	QFont font = volumeLabel_->font();
	font.setBold(true);
	volumeLabel_->setFont(font);
	mainLayout->addWidget(volumeLabel_);
	mainLayout->addWidget(volumeSlider_);
	mainLayout->setContentsMargins(0,4,0,4);
	mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setLayout(mainLayout);
	setMinimumHeight(140);
	setMinimumWidth(30);
	setWindowFlags(Qt::FramelessWindowHint);
	setMouseTracking(true);
	//Reading settings and alsa variables
	QSettings setts_;
	isLightStyle_ = setts_.value(ICOSTYLE, true).toBool();
	isAutorun_ = setts_.value(ISAUTO, false).toBool();
	//Creating settings dialog
	settingsDialog_ = new SettingsDialog(this);
#ifdef USE_PULSE
	isPulse_ = setts_.value(PULSE, false).toBool();
	pulse_ = new PulseCore(APP_NAME);
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
	alsaWork_ = new AlsaWork();
	cardList_ = alsaWork_->getCardsList();
	cardIndex_ = setts_.value(CARD_INDEX, 0).toInt();
	cardIndex_ = cardList_.size() >= cardIndex_ ? cardIndex_ : 0;
	QStringList mixers = alsaWork_->getVolumeMixers(cardIndex_);
	if (mixers.isEmpty()) { //check card for empty mixerlist
		for (int index = 0; index < cardList_.size(); ++index) {
			mixers = alsaWork_->getVolumeMixers(index);
			if (index != cardIndex_ && !mixers.isEmpty()) {
				mixerList_ = mixers;
				cardIndex_ = index;
				break;
			}
		}
	}
	else {
		mixerList_ = mixers;
	}
	QString mixer = mixerList_.contains("Master") ? "Master" : mixerList_.at(0);
	mixerName_ = setts_.value(MIXER_NAME, mixer).toString();
	cardName_ = alsaWork_->getCardName(cardIndex_);
	if (!isPulse_) {
		volumeValue_ = alsaWork_->getAlsaVolume(cardIndex_, mixerName_);
		isMuted_ = !alsaWork_->getMute(cardIndex_, mixerName_);
	}
	switchList_ = alsaWork_->getSwitchList(cardIndex_);
	updateSwitches();
	if (!isPulse_) {
		settingsDialog_->setSoundCards(cardList_);
		settingsDialog_->setCurrentCard(cardIndex_);
	} else {
		settingsDialog_->setSoundCards(pulseCardList_);
		settingsDialog_->setCurrentCard(pulse_->getCurrentDeviceIndex());
		settingsDialog_->setUsePulse(isPulse_);
		settingsDialog_->hideAlsaElements(isPulse_);
	}
	//
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
	//
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
#ifdef USE_PULSE
	delete pulse_;
#endif
	delete alsaWork_;
	delete restore_;
	delete settings_;
	delete mute_;
	delete about_;
	delete exit_;
	delete trayMenu_;
	delete trayIcon_;
	delete volumeSlider_;
	delete volumeLabel_;
	delete settingsDialog_;
}

void PopupWindow::createActions()
{
	restore_ = new QAction(tr("&Restore"), this);
	connect(restore_, SIGNAL(triggered()), this, SLOT(showPopup()));

	settings_ = new QAction(tr("&Settings"), this);
	connect(settings_, SIGNAL(triggered()), this, SLOT(showSettings()));

	mute_ = new QAction(tr("&Mute"), this);
	mute_->setCheckable(true);
	connect(mute_, SIGNAL(toggled(bool)), this, SLOT(onMute(bool)));

	about_ = new QAction(tr("&About..."), this);
	connect(about_, SIGNAL(triggered()), this, SLOT(onAbout()));

	exit_ = new QAction(tr("&Quit"), this);
	connect(exit_, SIGNAL(triggered()), this, SLOT(onQuit()));
}

void PopupWindow::createTrayMenu()
{
	trayMenu_ = new QMenu(this);
	trayMenu_->addAction(restore_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(settings_);
	trayMenu_->addAction(mute_);
	trayMenu_->addAction(about_);
	trayMenu_->addSeparator();
	trayMenu_->addAction(exit_);
}

void PopupWindow::onQuit()
{
	close();
}

void PopupWindow::onAbout()
{
	QString title = QString(tr("About QtAlsaVolume"));
#ifdef USE_PULSE
	QString msg = QString(tr("Tray Alsa Volume Changer written using Qt\n\nWith Pulseaudio support\n\n2014 (c) Vitaly Tonkacheyev (thetvg@gmail.com)\n\nversion: %1")).arg(APP_VERSION);
#else
	QString msg = QString(tr("Tray Alsa Volume Changer written using Qt\n\n2014 (c) Vitaly Tonkacheyev (thetvg@gmail.com)\n\nversion: %1")).arg(APP_VERSION);
#endif
	QMessageBox::about(this, title, msg);
}

void PopupWindow::showPopup()
{
	if (!this->isVisible()) {
		const int screenHeight = qApp->desktop()->availableGeometry().height();
		const int screenTop = qApp->desktop()->availableGeometry().top();
		QPoint point;
		const int iconLeft = trayIcon_->geometry().left();
		const int iconWidth = trayIcon_->geometry().width();
		const int iconTop = trayIcon_->geometry().top();
		const int windowWidth = minimumWidth();
		const int windowHeight = minimumHeight();
		int position = iconTop > screenHeight/2 ? BOTTOM : TOP;
		point.setX(iconLeft + iconWidth/2 - windowWidth/2);
		switch (position) {
		case TOP:
			point.setY(screenTop + 2);
			break;
		case BOTTOM:
			point.setY(screenHeight - windowHeight -2);
			break;
		default:
			break;
		}
		this->setGeometry(point.x(), point.y(), windowWidth, windowHeight);
		this->show();
	}
	else {
		this->hide();
	}
}

void PopupWindow::setTrayIcon(int value)
{
	QString pathPrefix;
	if (isLightStyle_) {
		pathPrefix = "icons/light/";
	}
	else {
		pathPrefix = "icons/dark/";
	}
	QString pathSuffix = "tb_icon100.png";
	if (value <= 0) {
		pathSuffix = "tb_icon0.png";
	}
	if (value >0 && (value < 40)) {
		pathSuffix = "tb_icon20.png";
	}
	if (value >=40 && (value < 60)) {
		pathSuffix = "tb_icon40.png";
	}
	if (value >=60 && (value < 80)) {
		pathSuffix = "tb_icon60.png";
	}
	if (value >=80 && (value < 100)) {
		pathSuffix = "tb_icon80.png";
	}
	if (value >= 100) {
		pathSuffix = "tb_icon100.png";
	}
	if (isMuted_) {
		pathSuffix = "tb_icon0.png";
	}
	QString fullPath = getResPath(pathPrefix + pathSuffix);
	trayIcon_->setIcon(QIcon(fullPath));
}

void PopupWindow::showSettings()
{
	settingsDialog_->setIconStyle(isLightStyle_);
#ifdef USE_PULSE
	if (isPulse_) {
		pulseCardList_ = pulse_->getCardList();
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
		alsaWork_->setMute(cardIndex_, mixerName_, !isToggled);
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

bool PopupWindow::eventFilter(QObject *object, QEvent *event)
{
	if (object == trayIcon_) {
		if (event->type() == QEvent::Wheel) {
			QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
			int degs = wheelEvent->delta()/8;
			int steps = degs/15;
			setVolume(steps);
			return true;
		}
	}
	return false;
}

void PopupWindow::mouseMoveEvent(QMouseEvent *event)
{
	const QPoint point = event->globalPos();
	const int top_ = this->geometry().top();
	const int left_ = this->geometry().left();
	const int right_ = this->geometry().right();
	if ((point.x() < left_ || point.x() > right_ || point.y() < top_) && this->isVisible()) {
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
		alsaWork_->setAlsaVolume(cardIndex_, mixerName_, value);
	}
	volumeValue_ = value;
	volumeLabel_->setText(QString::number(value));
	setTrayIcon(value);
	setIconToolTip(value);
}

void PopupWindow::setIconToolTip(int value)
{
	if (isPulse_) {
		const QString message = tr("Card: ") + pulse_->getDeviceDescription(pulseCardName_) + "\n"+ tr("Volume: ") + QString::number(value);
		trayIcon_->setToolTip(message);
	}
	else {
		const QString message = tr("Card: ") + cardName_ + "\n"+ tr("Mixer: ") + mixerName_ + "\n" + tr("Volume: ") + QString::number(value);
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
		cardIndex_ = (card >0 && card < cardList_.size()) ? card : 0;
		cardName_ = cardList_.at(cardIndex_);
		mixerList_ = alsaWork_->getVolumeMixers(cardIndex_);
		settingsDialog_->setMixers(mixerList_);
		switchList_ = alsaWork_->getSwitchList(cardIndex_);
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
		volumeValue_ = alsaWork_->getAlsaVolume(cardIndex_, mixer);
		volumeSlider_->setValue(volumeValue_);
	}
}

void PopupWindow::updateSwitches()
{
	if (!switchList_.isEmpty()) {
		playBackItems_ = switchList_.playbackSwitchList();
		captureItems_ = switchList_.captureSwitchList();
		enumItems_ = switchList_.enumSwitchList();
	}
}

void PopupWindow::onPlayback(const QString &name, bool isIt)
{
	alsaWork_->setSwitch(cardIndex_, name, PLAYBACK, isIt);
}

void PopupWindow::onCapture(const QString &name, bool isIt)
{
	alsaWork_->setSwitch(cardIndex_, name, CAPTURE ,isIt);
}

void PopupWindow::onEnum(const QString &name, bool isIt)
{
	alsaWork_->setSwitch(cardIndex_, name, ENUM, isIt);
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
	QStringList resDirs;
	resDirs << QString(QDir::currentPath());
	resDirs << QString(qApp->applicationDirPath());
	resDirs << QString("/usr/share/qtalsavolume");
	resDirs << QString("/usr/local/share/qtalsavolume");
	resDirs << QString(QDir::home().absolutePath() + "/.local/share/qtalsavolume");
	foreach(const QString &dir, resDirs){
		QString fullName = dir + "/" + fileName;
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
