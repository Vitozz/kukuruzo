#include "popupwindow.h"

#include <QtGui>
#include <QDesktopWidget>
#include <QDebug>

#define CARD_INDEX "Main/card"
#define MIXER_NAME "Main/mixer"

PopupWindow::PopupWindow()
{
	setWindowIcon(QIcon(":/images/icons/volume_ico.png"));
	alsaWork_ = new AlsaWork();
	createActions();
	createTrayMenu();
	trayIcon_ = new QSystemTrayIcon(this);
	trayIcon_->setContextMenu(trayMenu_);
	//
	QVBoxLayout *mainLayout = new QVBoxLayout;
	volumeSlider_ = new QSlider(Qt::Vertical, this);
	volumeSlider_->setMaximum(100);
	volumeSlider_->setMinimum(0);
	volumeSlider_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	volumeSlider_->setGeometry(volumeSlider_->x(), volumeSlider_->y(), 200, volumeSlider_->width());
	mainLayout->addWidget(volumeSlider_);
	setLayout(mainLayout);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	setWindowFlags(Qt::FramelessWindowHint);
	setMouseTracking(true);
	//
	setts_ = new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	cardList_ = alsaWork_->getCardsList();
	cardIndex_ = setts_->value(CARD_INDEX, 0).toInt();
	cardIndex_ = cardList_.size() >= cardIndex_ ? cardIndex_ : 0;
	mixerList_ = alsaWork_->getVolumeMixers(cardIndex_);
	QString mixer = mixerList_.contains("Master") ? "Master" : mixerList_.at(0);
	mixerName_ = setts_->value(MIXER_NAME, mixer).toString();
	cardName_ = alsaWork_->getCardName(cardIndex_);
	volumeValue_ = alsaWork_->getAlsaVolume(cardIndex_, mixerName_);
	isMuted_ = !alsaWork_->getMute(cardIndex_, mixerName_);
	switchList_ = alsaWork_->getSwitchList(cardIndex_);
	updateSwitches();
	//
	settingsDialog_ = new SettingsDialog(this);
	settingsDialog_->setSoundCards(cardList_);
	settingsDialog_->setCurrentCard(cardName_);
	settingsDialog_->setMixers(mixerList_);
	settingsDialog_->setCurrentMixer(mixerName_);
	settingsDialog_->connectSignals();
	connect(settingsDialog_, SIGNAL(soundCardChanged(QString)), this, SLOT(onCardChanged(QString)));
	connect(settingsDialog_, SIGNAL(mixerChanged(QString)), this, SLOT(onMixerChanged(QString)));
	connect(settingsDialog_, SIGNAL(playChanged(QString,bool)), this, SLOT(onPlayback(QString,bool)));
	connect(settingsDialog_, SIGNAL(captChanged(QString,bool)), this, SLOT(onCapture(QString,bool)));
	connect(settingsDialog_, SIGNAL(enumChanged(QString,bool)), this, SLOT(onEnum(QString,bool)));
	connect(settingsDialog_, SIGNAL(autorunChanged(bool)), this, SLOT(onAutorun(bool)));
	mute_->setChecked(isMuted_);
	volumeSlider_->setValue(volumeValue_);
	setIconToolTip(volumeValue_);
	setTrayIcon(volumeValue_);
	qDebug() << setts_->allKeys();
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(volumeSlider_, SIGNAL(valueChanged(int)), this, SLOT(onSlider(int)));
	trayIcon_->installEventFilter(this);
	trayIcon_->show();
}

PopupWindow::~PopupWindow()
{
	delete alsaWork_;
	delete restore_;
	delete settings_;
	delete mute_;
	delete about_;
	delete exit_;
	delete trayMenu_;
	delete trayIcon_;
	delete volumeSlider_;
	delete settingsDialog_;
	delete setts_;
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
	exit(0);
}

void PopupWindow::onAbout()
{
	QMessageBox::about(this, "About QtAlsaVolume", "Tray Alsa Volume Changer written using Qt\n\n2013 (c) Vitaly Tonkacheyev (thetvg@gmail.com)\n\nversion: 0.0.1");
}

void PopupWindow::showPopup()
{
	if (!this->isVisible()) {
		const int screenHeight = qApp->desktop()->availableGeometry().height();
		QPoint point;
		const int iconLeft = trayIcon_->geometry().left();
		const int iconWidth = trayIcon_->geometry().width();
		const int iconTop = trayIcon_->geometry().top();
		const int windowWidth = this->width();
		const int windowHeight = this->height();
		int position = iconTop > screenHeight/2 ? BOTTOM : TOP;
		point.setX(iconLeft + iconWidth/2 - windowWidth/2);
		switch (position) {
		case TOP:
			point.setY(windowHeight + 2);
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
	QString iconPath = ":/images/icons/tb_icon100.png";
	if (value <= 0) {
		iconPath = ":/images/icons/tb_icon0.png";
	}
	if (value >0 && (value < 40)) {
		iconPath = ":/images/icons/tb_icon20.png";
	}
	if (value >=40 && (value < 60)) {
		iconPath = ":/images/icons/tb_icon40.png";
	}
	if (value >=60 && (value < 80)) {
		iconPath = ":/images/icons/tb_icon60.png";
	}
	if (value >=80 && (value < 100)) {
		iconPath = ":/images/icons/tb_icon80.png";
	}
	if (value >= 100) {
		iconPath = ":/images/icons/tb_icon100.png";
	}
	if (isMuted_) {
		iconPath = ":/images/icons/tb_icon0.png";
	}
	trayIcon_->setIcon(QIcon(iconPath));
}

void PopupWindow::showSettings()
{
	settingsDialog_->setCurrentCard(cardName_);
	settingsDialog_->setCurrentMixer(mixerName_);
	updateSwitches();
	settingsDialog_->setPlaybackChecks(playBackItems_);
	settingsDialog_->setCaptureChecks(captureItems_);
	settingsDialog_->setEnumChecks(enumItems_);
	settingsDialog_->showNormal();
}

void PopupWindow::onMute(bool isToggled)
{
	isMuted_ = isToggled;
	alsaWork_->setMute(cardIndex_, mixerName_, !isToggled);
	setTrayIcon(volumeValue_);
}

void PopupWindow::closeEvent(QCloseEvent *)
{
	setts_->setValue(CARD_INDEX, cardIndex_);
	setts_->setValue(MIXER_NAME, mixerName_);
}

bool PopupWindow::eventFilter(QObject *object, QEvent *event)
{
	if (object == trayIcon_) {
		if (event->type() == QEvent::Wheel) {
			QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
			int degs = wheelEvent->delta()/8;
			int steps = degs/15;
			setVolume(steps);
		}
	}
}

void PopupWindow::mouseMoveEvent(QMouseEvent *event)
{
	const QPoint point = event->globalPos();
	const int top_ = this->geometry().top();
	const int left_ = this->geometry().left();
	const int right_ = this->geometry().right();
	if (point.x() < left_ || point.x() > right_ || point.y() < top_ && this->isVisible()) {
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
	alsaWork_->setAlsaVolume(cardIndex_, mixerName_, value);
	volumeValue_ = value;
	setTrayIcon(value);
	setIconToolTip(value);
}

void PopupWindow::setIconToolTip(int value)
{
	const QString message = tr("Card: ") + cardName_ + "\n"+ tr("Mixer: ") + mixerName_ + "\n" + tr("Volume: ") + QString::number(value);
	trayIcon_->setToolTip(message);
}

void PopupWindow::onCardChanged(const QString &card)
{
	cardName_ = card;
	cardIndex_ = cardList_.indexOf(card);
	mixerList_ = alsaWork_->getVolumeMixers(cardIndex_);
	settingsDialog_->setMixers(mixerList_);
	switchList_ = alsaWork_->getSwitchList(cardIndex_);
	updateSwitches();
	settingsDialog_->setPlaybackChecks(playBackItems_);
	settingsDialog_->setCaptureChecks(captureItems_);
	settingsDialog_->setEnumChecks(enumItems_);
}

void PopupWindow::onMixerChanged(const QString &mixer)
{
	if (mixerList_.contains(mixer)) {
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

void PopupWindow::onAutorun(bool isIt)
{
//
}
