/*
 * popupwindow.cpp
 * Copyright (C) 2013-2025 Vitaly Tonkacheyev
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

#include "popupwindow.h"
#include "defines.h"

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScreen>
#include <QSlider>
#include <QVBoxLayout>
#include <QtGui>

#ifdef ISDEBUG
#include <QDebug>
#endif

static const QLatin1String appLogoLight(":/images/Logo_light");
static const QLatin1String appLogoDark(":/images/Logo_dark");
static const QLatin1String autoStartPath(".config/autostart");
static const QString fName(QDir::home().absolutePath() +
                           "/.config/autostart/qtalsavolume.desktop");
static const QString dFile =
    QString("%1%2%3%4%5%6%7%8")
        .arg("[Desktop Entry]\n", "Encoding=UTF-8\n", "Name=QtAlsaVolume\n",
             "Exec=qtalsavolume\n",
             QString("%1%2%3").arg("Version=", APP_VERSION, "\n"),
             "Type=Application\n",
             "Comment=Changes the sound volume from the system tray\n",
             "X-LXQt-Need-Tray=true");
static const int POPUP_HEIGHT = 140;
static const int POPUP_WIDTH = 30;

PopupWindow::PopupWindow()
    : alsaWork_(AlsaWork::Ptr(new AlsaWork())),
#ifdef USE_PULSE
      pulse_(PulseCore::Ptr(new PulseCore(APP_NAME))), deviceIndex_(0),
#endif
      mixerName_(QString()), cardIndex_(0), mixerList_(QStringList()),
      switchList_(MixerSwitches::Ptr()), playBackItems_(QList<switcher>()),
      captureItems_(QList<switcher>()), enumItems_(QList<switcher>()),
      trayIcon_(new TrayIcon()), mainLayout_(new QVBoxLayout()),
      volumeSlider_(new QSlider(Qt::Vertical, this)),
      volumeLabel_(new QLabel(this)), pollingTimer_(new QTimer(this)),
      settingsDialog_(new SettingsDialog()), cardName_(QString()),
      pulseCardName_(QString()), volumeValue_(0), pollingVolume_(0),
      isMuted_(false), isAutorun_(false), isLightStyle_(false), isPulse_(false),
      isPoll_(true), title_(tr("About QtAlsaVolume")),
      message_(
          QString(tr("<!DOCTYPE html><html><body>"
                     "<p><b>Tray Alsa Volume Changer written using Qt</b></p>"
#ifdef USE_PULSE
                     "<p>With Pulseaudio support</p>"
#endif
                     "<p>2013-2025 (c) Vitaly Tonkacheyev <address><a "
                     "href=\"mailto:thetvg@gmail.com\">&lt;EMail&gt;</a></"
                     "address></p>"
                     "<a "
                     "href=\"https://github.com/Vitozz/kukuruzo/tree/master/qt/"
                     "qtalsavolume"
                     "\">Program WebSite</a>"
                     "<p>version: <b>%1</b></p></body></html>"))
              .arg(APP_VERSION)) {
  // Start of tray icon initialization
  const QString errorHeader(tr("Error"));
  const QString systrayMissing(tr("System tray is not available"));
  if (!trayIcon_->isAvailable()) {
    QMessageBox::critical(this, errorHeader, systrayMissing);
    onQuit();
  }
  // Adding QLabel and QSlider to PopupWindow
  volumeSlider_->setRange(0, 100);
  volumeSlider_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  volumeLabel_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  QFont font(volumeLabel_->font());
  font.setBold(true);
  volumeLabel_->setFont(font);
  mainLayout_->addWidget(volumeLabel_);
  mainLayout_->addWidget(volumeSlider_);
  mainLayout_->setContentsMargins(0, 4, 0, 4);
  mainLayout_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  setLayout(mainLayout_);
  setMinimumHeight(POPUP_HEIGHT);
  setMinimumWidth(POPUP_WIDTH);
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  // Reading settings and alsa variables
  QSettings setts_;
  isLightStyle_ = setts_.value(ICOSTYLE, true).toBool();
  isAutorun_ = setts_.value(ISAUTO, false).toBool();
  isPoll_ = setts_.value(ISPOLL, true).toBool();
  setWindowIcon(QIcon(isLightStyle_ ? appLogoLight : appLogoDark));
#ifdef USE_PULSE
  const QString pulseIsMissing(
      tr("Can't start PulseAudio. Using Alsa by default"));
  isPulse_ = setts_.value(PULSE, false).toBool();
  if (pulse_->available()) {
    const QString lastSink = setts_.value(LAST_SINK, "").toString();
    pulseCardName_ = (!lastSink.isEmpty()) ? lastSink : pulse_->defaultSink();
    pulse_->setCurrentDevice(pulseCardName_);
    deviceIndex_ = pulse_->getCurrentDeviceIndex();
    if (isPulse_) {
      isMuted_ = pulse_->getMute();
      volumeValue_ = pulse_->getVolume();
    }
  } else {
    QMessageBox::critical(this, errorHeader, pulseIsMissing);
    pulse_.clear();
    isPulse_ = false;
    settingsDialog_->setPulseAvailable(false);
  }
#else
  isPulse_ = false;
  settingsDialog_->setPulseAvailable(false);
#endif
  cardIndex_ = setts_.value(CARD_INDEX, 0).toInt();
  if (alsaWork_->cardExists(cardIndex_)) {
    alsaWork_->setCurrentCard(cardIndex_);
    if (!alsaWork_->haveVolumeMixers()) {
      cardIndex_ = alsaWork_->getFirstCardWithMixers();
      alsaWork_->setCurrentCard(cardIndex_);
    }
  }
  mixerList_ = alsaWork_->getVolumeMixers();
  QString mixer(mixerList_.contains("Master") ? "Master" : mixerList_.at(0));
  mixerName_ = setts_.value(MIXER_NAME, mixer).toString();
  cardName_ = alsaWork_->getCardName(cardIndex_);
  if (!isPulse_) {
    volumeValue_ = alsaWork_->getAlsaVolume();
    isMuted_ = alsaWork_->getMute();
  }
  updateSwitches();
  if (!isPulse_) {
    settingsDialog_->setSoundCards(alsaWork_->getCardsList());
    settingsDialog_->setCurrentCard(cardIndex_);
  }
#ifdef USE_PULSE
  else if (pulse_) {
    settingsDialog_->setSoundCards(pulse_->getCardList());
    settingsDialog_->setCurrentCard(pulse_->getCurrentDeviceIndex());
    settingsDialog_->setUsePulse(isPulse_);
    settingsDialog_->hideAlsaElements(isPulse_);
  }
#endif
  settingsDialog_->setWindowIcon(
      QIcon(isLightStyle_ ? appLogoLight : appLogoDark));
  settingsDialog_->setIconStyle(isLightStyle_);
  settingsDialog_
      ->connectSignals(); // connecting settingsDialog_ internal signals
  connect(settingsDialog_, &SettingsDialog::soundCardChanged, this,
          &PopupWindow::onCardChanged);
  connect(settingsDialog_, &SettingsDialog::mixerChanged, this,
          &PopupWindow::onMixerChanged);
  connect(settingsDialog_, &SettingsDialog::playChanged, this,
          &PopupWindow::onPlayback);
  connect(settingsDialog_, &SettingsDialog::captChanged, this,
          &PopupWindow::onCapture);
  connect(settingsDialog_, &SettingsDialog::enumChanged, this,
          &PopupWindow::onEnum);
  connect(settingsDialog_, &SettingsDialog::autorunChanged, this,
          &PopupWindow::onAutorun);
  connect(settingsDialog_, &SettingsDialog::styleChanged, this,
          &PopupWindow::onStyleChanged);
  connect(settingsDialog_, &SettingsDialog::timerEnabled, this,
          &PopupWindow::enablePolling);
#ifdef USE_PULSE
  if (pulse_) {
    connect(settingsDialog_, &SettingsDialog::soundSystemChanged, this,
            &PopupWindow::onSoundSystem);
  }
#endif
  createDesktopFile();
  // Finish of tray icon initialization
  pollingVolume_ = volumeValue_;
  volumeSlider_->setValue(volumeValue_);
  volumeLabel_->setText(QString::number(volumeValue_));
  setTrayIcon(volumeValue_);
  setIconToolTip(volumeValue_);
  connect(trayIcon_, &TrayIcon::activated, this, &PopupWindow::iconActivated);
  connect(trayIcon_, &TrayIcon::muted, this, &PopupWindow::onMute);
  trayIcon_->setMute(isMuted_);
  connect(volumeSlider_, &QSlider::valueChanged, this, &PopupWindow::onSlider);
  installEventFilter(this);
  pollingTimer_->setInterval(POLLING_INTERVAL);
  connect(pollingTimer_, &QTimer::timeout, this, &PopupWindow::onTimeout);
  pollingTimer_->start();
}

PopupWindow::~PopupWindow() {
  saveSettings();
  delete settingsDialog_;
  if (pollingTimer_->isActive()) {
    pollingTimer_->stop();
  }
  delete pollingTimer_;
  delete volumeLabel_;
  delete volumeSlider_;
  delete mainLayout_;
  delete trayIcon_;
}

void PopupWindow::onAbout() {
  QMessageBox about;
  about.setIconPixmap(QPixmap(isLightStyle_ ? appLogoLight : appLogoDark));
  about.setWindowIcon(windowIcon());
  about.setText(message_);
  about.setWindowTitle(title_);
  about.exec();
}

void PopupWindow::showPopup() {
  if (!this->isVisible()) {
    this->show();
    setPopupPosition();
  } else {
    this->hide();
  }
}

void PopupWindow::setTrayIcon(int value) {
  const QString pathPrefix((isLightStyle_) ? ":/images/icons/light/"
                                           : ":/images/icons/dark/");
  value = (value <= 0) ? 0 : (value > 100) ? 100 : value;
  int number = (value < 10) ? 10 : int(value / 10) * 10;
  if (value <= 0) {
    number = 0;
  }
  if (isMuted_) {
    number = 0;
  }
  const QString fullPath(
      QString("%1tb_icon%2.png").arg(pathPrefix, QString::number(number)));
#ifdef ISDEBUG
  qDebug() << "val " << value;
  qDebug() << "num " << number;
  qDebug() << "Icon path " << fullPath;
#endif
  trayIcon_->setTrayIcon(fullPath);
}

void PopupWindow::showSettings() {
  settingsDialog_->setWindowIcon(
      QIcon(isLightStyle_ ? appLogoLight : appLogoDark));
  settingsDialog_->setIconStyle(isLightStyle_);
  settingsDialog_->setUsePolling(isPoll_);
#ifdef USE_PULSE
  if (isPulse_ && pulse_) {
    settingsDialog_->blockSignals(true);
    settingsDialog_->setSoundCards(pulse_->getCardList());
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

void PopupWindow::onMute(bool isToggled) {
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

void PopupWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  event->accept();
#ifdef ISDEBUG
  qDebug() << "CloseEvent accepted";
#endif
  QCoreApplication::quit();
}

void PopupWindow::saveSettings() {
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
}

void PopupWindow::setPopupPosition() {
  const QRect trayGeometry(trayIcon_->iconGeometery());
  QPoint to;
  QPoint point(trayIcon_->iconPosition());
  Position position;
  const int screenHeight = screen()->availableGeometry().height();
  const int screenTop = screen()->availableGeometry().top();
  if (!trayGeometry.isEmpty()) {
    position = trayGeometry.top() > screenHeight / 2 ? BOTTOM : TOP;
    to.setX(trayGeometry.left() + trayGeometry.width() / 2 - width() / 2);
  } else {
    position = (point.y() > screenHeight / 2) ? BOTTOM : TOP;
    to.setX(point.x() - width() / 2);
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
#ifdef ISDEBUG
  qDebug() << "TO = " << to;
#endif
  this->setGeometry(to.x(), to.y(), width(), height());
}

bool PopupWindow::eventFilter(QObject *object, QEvent *event) {
  if (object == this) {
    if (event->type() == QEvent::Leave ||
        event->type() == QEvent::WindowDeactivate) {
#ifdef ISDEBUG
      qDebug() << "Mouse Leave";
#endif
      this->hide();
      return true;
    }
  }
  return false;
}

void PopupWindow::setVolume(int value) {
  volumeValue_ += value * DELTA_VOLUME;
  volumeValue_ = (volumeValue_ <= 0)    ? 0
                 : (volumeValue_ > 100) ? 100
                                        : volumeValue_;
  volumeSlider_->setValue(volumeValue_);
}

void PopupWindow::onSlider(int value) {
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

void PopupWindow::setIconToolTip(int value) {
#ifdef USE_PULSE
  if (isPulse_) {
    if (pulse_) {
      const QString message =
          tr("Card: %1%2%3")
              .arg(pulse_->getDeviceDescription(pulseCardName_), "\n",
                   tr("Volume: %1").arg(QString::number(value)));
      trayIcon_->setToolTip(message);
    }
  }
#endif
  if (!isPulse_) {
    const QString message =
        tr("Card: %1%2%3%4")
            .arg(cardName_, "\n", tr("Mixer: %1%2").arg(mixerName_, "\n"),
                 tr("Volume: %1").arg(QString::number(value)));
    trayIcon_->setToolTip(message);
  }
}

void PopupWindow::onCardChanged(int card) {
#ifdef USE_PULSE
  if (isPulse_ && pulse_) {
    const QString oldCard(pulseCardName_);
    pulseCardName_ = pulse_->getDeviceNameByIndex(card);
    pulse_->setCurrentDevice(pulseCardName_);
    settingsDialog_->blockSignals(true); // Block Signals
    settingsDialog_->setSoundCards(pulse_->getCardList());
    settingsDialog_->setCurrentCard(pulse_->getCurrentDeviceIndex());
    settingsDialog_->blockSignals(false); // Block Signals
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

void PopupWindow::onMixerChanged(const QString &mixer) {
  if (mixerList_.contains(mixer) && !isPulse_) {
    mixerName_ = mixer;
    alsaWork_->setCurrentMixer(mixerList_.indexOf(mixerName_));
    volumeValue_ = alsaWork_->getAlsaVolume();
    volumeSlider_->setValue(volumeValue_);
  }
}

void PopupWindow::updateAlsa(int card) {
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

void PopupWindow::updateSwitches() {
  switchList_ = alsaWork_->getSwitchList();
  if (switchList_ && !switchList_->isEmpty()) {
    playBackItems_ = switchList_->playbackSwitchList();
    captureItems_ = switchList_->captureSwitchList();
    enumItems_ = switchList_->enumSwitchList();
  }
}

void PopupWindow::onPlayback(const QString &name, bool isIt) {
  alsaWork_->setSwitch(name, PLAYBACK, isIt);
}

void PopupWindow::onCapture(const QString &name, bool isIt) {
  alsaWork_->setSwitch(name, CAPTURE, isIt);
}

void PopupWindow::onEnum(const QString &name, bool isIt) {
  alsaWork_->setSwitch(name, ENUM, isIt);
}

void PopupWindow::iconActivated(ActivationReason reason) {
  switch (reason) {
  case ABOUT:
    onAbout();
    break;
  case ABOUTQT:
    QApplication::aboutQt();
    break;
  case RESTORE:
    showPopup();
    break;
  case SETTINGS:
    showSettings();
    break;
  case EXIT:
    onQuit();
    break;
  case WHEELUP:
    setVolume(1);
    break;
  case WHEELDOWN:
    setVolume(-1);
    break;
  }
}

void PopupWindow::onAutorun(bool isIt) {
  if (isAutorun_ != isIt) {
    isAutorun_ = isIt;
    QSettings setts;
    setts.setValue(ISAUTO, isAutorun_);
    setts.sync();
    createDesktopFile();
  }
}

void PopupWindow::createDesktopFile() const {
  QDir home = QDir::home();
  if (!home.exists(autoStartPath)) {
    home.mkpath(autoStartPath);
  }
  QFile f(fName);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
    f.write(dFile.trimmed().toUtf8());
    f.write(
        QString("\nHidden=%1").arg(isAutorun_ ? "false\n" : "true\n").toUtf8());
  }
}

void PopupWindow::readDesktopFile() {
  QFile desktop(fName);
  if (desktop.open(QIODevice::ReadOnly)) {
    auto regexp = QRegularExpression(R"(\bhidden\s*=\s*false)",
                                     QRegularExpression::CaseInsensitiveOption);
    settingsDialog_->setAutorun(QString(desktop.readAll()).contains(regexp));
  }
}

void PopupWindow::onStyleChanged(bool isLight) {
  isLightStyle_ = isLight;
  setTrayIcon(volumeValue_);
}

void PopupWindow::onTimeout() {
  if (isPoll_) {
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
          trayIcon_->setMute(isMuted_);
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
        trayIcon_->setMute(isMuted_);
      }
    }
#endif
  }
}

void PopupWindow::onSoundSystem(bool isIt) {
  isPulse_ = isIt;
  if (isPulse_) {
#ifdef USE_PULSE
    if (pulse_) {
      settingsDialog_->hideAlsaElements(isPulse_);
      settingsDialog_->setSoundCards(pulse_->getCardList());
      settingsDialog_->setCurrentCard(deviceIndex_);
    }
#endif
  } else {
    settingsDialog_->hideAlsaElements(isPulse_);
    settingsDialog_->setSoundCards(alsaWork_->getCardsList());
    settingsDialog_->setCurrentCard(cardIndex_);
  }
}

void PopupWindow::enablePolling(bool isIt) { isPoll_ = isIt; }
