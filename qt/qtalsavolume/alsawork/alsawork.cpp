/*
 * alsawork.cpp
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

#include "mixerswitches.h"
#include "volumemixers.h"
#include "alsawork.h"
#include <stdexcept>

//#include <QDebug>

const double ZERO = 0.0;

AlsaWork::AlsaWork()
{
	switches_ = new MixerSwitches();
	volumeMixers_ = new VolumeMixers();
}

AlsaWork::~AlsaWork()
{
	delete switches_;
	delete volumeMixers_;
	snd_config_update_free_global();
}

//public
void AlsaWork::setAlsaVolume(int cardId, const QString &mixer, int volume)
{
	if (checkCardId(cardId) && mixerList_.contains(mixer)) {
		snd_mixer_t *handle = getMixerHanlde(cardId);
		snd_mixer_elem_t *element = initMixerElement(handle, mixer.toUtf8().data());
		setVolume(element, handle, double(volume));
	}
}

int AlsaWork::getAlsaVolume(int cardId, const QString &mixer)
{
	if (mixerList_.contains(mixer)) {
		snd_mixer_t *handle = getMixerHanlde(cardId);
		snd_mixer_elem_t *elem = initMixerElement(handle, mixer.toUtf8().data());
		long minv, maxv, outvol;
		snd_mixer_selem_channel_id_t chanelid = checkMixerChannels(elem);
		if (snd_mixer_selem_has_playback_volume(elem) || snd_mixer_selem_has_playback_volume_joined(elem)) {
			checkError(snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv));
			if (snd_mixer_selem_has_playback_channel(elem, chanelid)) {
				checkError(snd_mixer_selem_get_playback_volume(elem, chanelid, &outvol));
			}
			if ((maxv - minv) != 0) {
				return int(100*(outvol - minv)/(maxv-minv));
			}
		}
		if (snd_mixer_selem_has_capture_volume(elem) || snd_mixer_selem_has_capture_volume_joined(elem)) {
			checkError(snd_mixer_selem_get_capture_volume_range(elem, &minv, &maxv));
			if (snd_mixer_selem_has_capture_channel(elem, chanelid)) {
				checkError(snd_mixer_selem_get_capture_volume(elem, chanelid, &outvol));
			}
			if ((maxv - minv) != 0) {
				return int(100*(outvol - minv)/(maxv-minv));
			}
		}
		checkError(snd_mixer_close(handle));
	}
	return int(ZERO);
}

QString AlsaWork::getCardName(int index)
{
	QString card(formatCardName(index));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.toUtf8().data(), SND_CTL_NONBLOCK));
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	checkError(snd_ctl_card_info(ctl, cardInfo));
	const char *cardName = snd_ctl_card_info_get_name(cardInfo);
	return QString(cardName);
}

QStringList &AlsaWork::getCardsList()
{
	getCards();
	return cardList_;
}

QStringList &AlsaWork::getVolumeMixers(int cardIndex)
{
	updateMixerList(cardIndex);
	return mixerList_;
}

void AlsaWork::updateMixerList(int cardIndex)
{
	updateMixers(cardIndex);
	if (!mixerList_.empty())
		mixerList_.clear();
	QStringList plist = volumeMixers_->playback();
	QStringList clist = volumeMixers_->capture();
	foreach (QString item, plist) {
		mixerList_.push_back(item);
	}
	foreach (QString item, clist) {
		mixerList_.push_back(item);
	}
}

MixerSwitches &AlsaWork::getSwitchList(int cardIndex)
{
	updateMixers(cardIndex);
	return *switches_;
}

bool AlsaWork::checkCardId(int cardId)
{
	try {
		if (cardId < int(cardList_.size()) && !cardList_.at(cardId).isEmpty()) {
			return true;
		}
	}
	catch (std::out_of_range &ex) {
		std::cerr << "alsawork.cpp::124:: Item out of Range " << ex.what() << std::endl;
	}
	return false;
}

void AlsaWork::setSwitch(int cardId, const QString &mixer, int id, bool enabled)
{
	snd_mixer_t *handle = getMixerHanlde(cardId);
	snd_mixer_elem_t* elem = initMixerElement(handle, mixer.toUtf8().data());
	switch (id) {
	case PLAYBACK:
		checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
		break;
	case CAPTURE:
		checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
		break;
	case ENUM:
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);
		checkError(snd_mixer_selem_set_enum_item(elem, channel, uint(enabled)));
		break;
	}
	checkError(snd_mixer_close(handle));
}

void AlsaWork::setMute(int cardId, const QString &mixer, bool enabled)
{
	if (mixerList_.contains(mixer)) {
		snd_mixer_t *handle = getMixerHanlde(cardId);
		snd_mixer_elem_t* elem = initMixerElement(handle, mixer.toUtf8().data());
		if (snd_mixer_selem_has_playback_switch(elem)
		   || snd_mixer_selem_has_playback_switch_joined(elem)) {
			checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
		}
		if (snd_mixer_selem_has_capture_switch(elem)
		    || snd_mixer_selem_has_common_switch(elem)
		    || snd_mixer_selem_has_capture_switch_joined(elem)
		    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
			checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
		}
		checkError(snd_mixer_close(handle));
	}
}

bool AlsaWork::getMute(int cardId, const QString &mixer)
{
	if (mixerList_.contains(mixer)) {
		snd_mixer_t *handle = getMixerHanlde(cardId);
		snd_mixer_elem_t* elem = initMixerElement(handle, mixer.toUtf8().data());
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);

		if (snd_mixer_selem_has_playback_switch(elem)
		    || snd_mixer_selem_has_playback_switch_joined(elem)) {
			int value = 0;
			checkError(snd_mixer_selem_get_playback_switch(elem, channel, &value));
			return bool(value);
		}
		if (snd_mixer_selem_has_capture_switch(elem)
		    || snd_mixer_selem_has_common_switch(elem)
		    || snd_mixer_selem_has_capture_switch_joined(elem)
		    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
			int value = 0;
			checkError(snd_mixer_selem_get_capture_switch(elem, channel, &value));
			return bool(value);
		}
		checkError(snd_mixer_close(handle));
	}
	return true;
}
//private
void AlsaWork::checkError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
}

snd_mixer_elem_t *AlsaWork::initMixerElement(snd_mixer_t *handle, const char *mixer)
{
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	snd_mixer_selem_id_set_index(smid, 0);
	snd_mixer_selem_id_set_name(smid, mixer);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, smid);
	return elem;
}

void AlsaWork::setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume)
{
	if (snd_mixer_selem_has_playback_volume(element)) {
		long min, max;
		checkError(snd_mixer_selem_get_playback_volume_range(element, &min, &max));
		long volume_ = long(volume)*(max - min) / 100;
		checkError(snd_mixer_selem_set_playback_volume_all(element, volume_));
	}
	else if (snd_mixer_selem_has_capture_volume(element)) {
		long min, max;
		checkError(snd_mixer_selem_get_capture_volume_range(element, &min, &max));
		long volume_ = long(volume)*(max - min) / 100;
		checkError(snd_mixer_selem_set_capture_volume_all(element, volume_));
	}
	else{
		std::cerr << "Selected mixer has no playback or capture volume" << std::endl;
	}
	checkError(snd_mixer_close(handle));
}

snd_mixer_t *AlsaWork::getMixerHanlde(int id)
{
	QString card(formatCardName(id));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.toLatin1().data(), SND_CTL_NONBLOCK));
	snd_hctl_t *hctl;
	checkError(snd_hctl_open_ctl(&hctl, ctl));
	snd_mixer_t *handle;
	checkError(snd_mixer_open(&handle, 0));
	checkError(snd_mixer_attach_hctl(handle, hctl));
	checkError(snd_mixer_selem_register(handle, NULL, NULL));
	checkError(snd_mixer_load(handle));
	return handle;
}

QString AlsaWork::formatCardName(int id)
{
	QString result = "hw:" + QString::number(id);
	return result;
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

void AlsaWork::updateMixers(int cardIndex)
{
	//clearing lists
	volumeMixers_->playBackClear();
	volumeMixers_->captureClear();
	switches_->clear(CAPTURE);
	switches_->clear(PLAYBACK);
	switches_->clear(ENUM);
	//
	snd_mixer_t *handle = getMixerHanlde(cardIndex);
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	QString name;
	for (snd_mixer_elem_t *element = snd_mixer_first_elem(handle);
	     element;
	     element = snd_mixer_elem_next(element)) {
		switcher sCap;
		snd_mixer_selem_get_id(element, smid);
		name = snd_mixer_selem_id_get_name(smid);
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(element);

		if (snd_mixer_selem_has_playback_volume(element)
		    || snd_mixer_selem_has_playback_volume_joined(element)
		    || snd_mixer_selem_has_common_volume(element)) {
			volumeMixers_->pushBack(PLAYBACK, name);
		}
		if (snd_mixer_selem_has_capture_volume(element)
		    || snd_mixer_selem_has_capture_volume_joined(element)) {
			volumeMixers_->pushBack(CAPTURE, name);
		}
		if (snd_mixer_selem_has_capture_switch(element)
		    || snd_mixer_selem_has_common_switch(element)
		    || snd_mixer_selem_has_capture_switch_joined(element)
		    || snd_mixer_selem_has_capture_switch_exclusive(element)){
			int value = 0;
			checkError(snd_mixer_selem_get_capture_switch(element, channel, &value));
			//qDebug() << "CP "<< name;
			sCap = switcher(name, bool(value));
			switches_->pushBack(CAPTURE, sCap);
		}
		if (snd_mixer_selem_has_playback_switch(element)
		    || snd_mixer_selem_has_playback_switch_joined(element)){
			int value = 0;
			checkError(snd_mixer_selem_get_playback_switch(element, channel, &value));
			//qDebug() << "PL "<< name;
			sCap = switcher(name, bool(value));
			switches_->pushBack(PLAYBACK, sCap);
		}
		if (snd_mixer_selem_is_enumerated(element)) {
			uint value = 0;
			checkError(snd_mixer_selem_get_enum_item(element, channel, &value));
			//qDebug() << "EN "<< name;
			sCap = switcher(name, bool(value));
			switches_->pushBack(ENUM, sCap);
		}
	}
	checkError(snd_mixer_close(handle));
}

void AlsaWork::getCards()
{
	if (!cardList_.empty())
		cardList_.clear();
	int total = getTotalCards();
	if (total >= 1) {
		for (int card = 0; card < total; card++) {
			QString cname(getCardName(card));
			cardList_.push_back(cname);
		}
	}
}

snd_mixer_selem_channel_id_t AlsaWork::checkMixerChannels(snd_mixer_elem_t *element)
{
	if (snd_mixer_selem_is_playback_mono(element)) {
		return SND_MIXER_SCHN_MONO;
	}
	else {
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
			if (snd_mixer_selem_has_playback_channel(element, (snd_mixer_selem_channel_id_t)channel)) {
				return (snd_mixer_selem_channel_id_t)channel;
			}
		}
	}
	if (snd_mixer_selem_is_capture_mono(element)) {
		return SND_MIXER_SCHN_MONO;
	}
	else {
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
			if (snd_mixer_selem_has_capture_channel(element, (snd_mixer_selem_channel_id_t)channel)) {
				return (snd_mixer_selem_channel_id_t)channel;
			}
		}
	}
	return SND_MIXER_SCHN_UNKNOWN;
}