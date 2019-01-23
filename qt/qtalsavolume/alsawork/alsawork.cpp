/*
 * alsawork.cpp
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
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

#include "mixerswitches.h"
#include "alsawork.h"
#include <QMessageBox>
#include <QString>
#ifdef ISDEBUG
#include <QDebug>
#endif

static const QString ERROR_TITLE = "Error in alsawork.cpp";

AlsaWork::AlsaWork()
    : cardList_(QStringList())
{
    getCards();
    foreach (const QString &name, cardList_) {
        devices_.push_back(AlsaDevice::Ptr(new AlsaDevice(cardList_.indexOf(name), name)));
    }
    setCurrentCard(0);
}

AlsaWork::~AlsaWork()
{
    snd_config_update_free_global();
}

//public
void AlsaWork::setCurrentCard(int cardId)
{
    if(cardId < devices_.size()) {
        currentAlsaDevice_ = devices_.at(cardId);
    }
}

void AlsaWork::setCurrentMixer(const QString &mixer)
{
    currentAlsaDevice_->setCurrentMixer(mixer);
}

void AlsaWork::setCurrentMixer(int id)
{
    currentAlsaDevice_->setCurrentMixer(id);
}

void AlsaWork::setAlsaVolume(int volume)
{
    currentAlsaDevice_->setDeviceVolume(static_cast<double>(volume));
}

int AlsaWork::getAlsaVolume()
{
    getCards();
    if (cardExists(currentAlsaDevice_->id())) {
        return int(currentAlsaDevice_->getVolume());
    }
    return 0;
}

const QString AlsaWork::getCardName(int index)
{
    QString card(AlsaDevice::formatCardName(index));
    snd_ctl_t *ctl;
    int res = snd_ctl_open(&ctl, card.toLocal8Bit(), SND_CTL_NONBLOCK);
    checkError(res);
    if (res == 0) {
        snd_ctl_card_info_t *cardInfo;
        snd_ctl_card_info_alloca(&cardInfo);
        res = snd_ctl_card_info(ctl, cardInfo);
        checkError(res);
        if(res == 0) {
            const char *cardName = snd_ctl_card_info_get_name(cardInfo);
            checkError(snd_ctl_close(ctl));
            return QString::fromLocal8Bit(cardName);
        }
    }
    return QString();
}

QString AlsaWork::getCurrentMixerName() const
{
    return currentAlsaDevice_->currentMixer();
}

const QStringList &AlsaWork::getCardsList() const
{
    return cardList_;
}

const QStringList &AlsaWork::getVolumeMixers() const
{
    return currentAlsaDevice_->mixers();
}

MixerSwitches::Ptr AlsaWork::getSwitchList() const
{
    return currentAlsaDevice_->switches();
}

void AlsaWork::setMute(bool enabled)
{
    currentAlsaDevice_->setMute(enabled);
}

bool AlsaWork::getMute()
{
    getCards();
    if (cardExists(currentAlsaDevice_->id())) {
        return !currentAlsaDevice_->getMute();
    }
    return false;
}

void AlsaWork::setSwitch(const QString &mixer, int id, bool enabled)
{
    currentAlsaDevice_->setSwitch(mixer, id, enabled);
}

bool AlsaWork::checkCardId(int cardId)
{
    if (cardId < int(cardList_.size()) && !cardList_.at(cardId).isEmpty()) {
        return true;
    }
    checkError(ERROR_TITLE,QString("line::119::Item with id=%1 out of Range ").arg(QString::number(cardId)));
    return false;
}

//private
void AlsaWork::checkError (int errorIndex)
{
    if (errorIndex < 0) {
        QMessageBox::critical(nullptr, ERROR_TITLE, QString::fromLocal8Bit(snd_strerror(errorIndex)));
    }
}

void AlsaWork::checkError(const QString &title, const QString &message)
{
    if(!title.isEmpty() && !message.isEmpty()) {
        QMessageBox::critical(nullptr, title, message);
    }
}

int AlsaWork::getTotalCards()
{
    int cards = 0;
    int index = -1;
    while (true) {
        int res = snd_card_next(&index);
        checkError(res);
        if (res == 0 && index < 0) {
            break;
        }
        ++cards;
    }
    return cards;
}

void AlsaWork::getCards()
{
    if (!cardList_.isEmpty())
        cardList_.clear();
    totalCards_ = getTotalCards();
#ifdef ISDEBUG
    qDebug() << "TC" << totalCards_;
#endif
    if (totalCards_ >= 1) {
        for (int card = 0; card < totalCards_; card++) {
            QString name(getCardName(card));
            if(!name.isEmpty()) {
                cardList_ << getCardName(card);
            }
        }
    }
}

bool AlsaWork::haveVolumeMixers()
{
    return currentAlsaDevice_->haveMixers();
}

bool AlsaWork::cardExists(int id)
{
    if (id >= 0 && id < totalCards_) {
        return true;
    }
    return false;
}

bool AlsaWork::mixerExists(const QString &name)
{
    int index = currentAlsaDevice_->mixers().indexOf(name);
    if (index >= 0) {
        return true;
    }
    return false;
}

bool AlsaWork::mixerExists(int id)
{
    return bool(id >=0 && id < currentAlsaDevice_->mixers().size());
}

int AlsaWork::getFirstCardWithMixers() const
{
    for(int index = 0; index < devices_.size(); index+=1) {
        if(devices_.at(index)->haveMixers()) {
            return index;
        }
    }
    return 0;
}

int AlsaWork::getCurrentMixerId() const
{
    return currentAlsaDevice_->currentMixerId();
}
