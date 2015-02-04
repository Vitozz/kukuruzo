/*
 * alsawork.cpp
 * Copyright (C) 2012-2014 Vitaly Tonkacheyev
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

#include "mixerswitches.h"
#include "alsawork.h"
#include <QMessageBox>

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
	if(cardId < (int)devices_.size()) {
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
	return int(currentAlsaDevice_->getVolume());
}

const QString AlsaWork::getCardName(int index)
{
	const QString card(AlsaDevice::formatCardName(index));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.toStdString().c_str(), SND_CTL_NONBLOCK));
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	checkError(snd_ctl_card_info(ctl, cardInfo));
	const char *cardName = snd_ctl_card_info_get_name(cardInfo);
	return QString::fromLocal8Bit(cardName);
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

const MixerSwitches &AlsaWork::getSwitchList() const
{
	return currentAlsaDevice_->switches();
}

void AlsaWork::setMute(bool enabled)
{
	currentAlsaDevice_->setMute(enabled);
}

bool AlsaWork::getMute()
{
	return !currentAlsaDevice_->getMute();
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
		QMessageBox::critical(0, ERROR_TITLE, QString::fromLocal8Bit(snd_strerror(errorIndex)));
	}
}

void AlsaWork::checkError(const QString &title, const QString &message)
{
	if(!title.isEmpty() && !message.isEmpty()) {
		QMessageBox::critical(0, title, message);
	}
}

int AlsaWork::getTotalCards()
{
	int cards = 0;
	int index = -1;
	while (true) {
		checkError(snd_card_next(&index));
		if (index < 0) {
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
	if (totalCards_ >= 1) {
		for (int card = 0; card < totalCards_; card++) {
			cardList_ << getCardName(card);
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
	return bool(id >=0 && id < (int)currentAlsaDevice_->mixers().size());
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
