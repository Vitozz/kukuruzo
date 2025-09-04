/*
 * alsadevice.cpp
 * Copyright (C) 2014-2025 Vitaly Tonkacheyev
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
#include "alsadevice.h"
#include <QMessageBox>
#ifdef ISDEBUG
#include <QDebug>
#include <utility>
#endif

#define ERROR_TITLE "Error in alsadevice.cpp"
#define ZERO 0.0

AlsaDevice::AlsaDevice(int id, QString card)
    : id_(id), name_(std::move(card)), volumeMixers_(QStringList()),
      captureMixers_(QStringList()), mixers_(QStringList()),
      switches_(MixerSwitches::Ptr(new MixerSwitches())), currentMixerId_(0),
      currentMixerName_(QString()) {
  updateElements();
}

void AlsaDevice::updateElements() {
  if (!volumeMixers_.isEmpty()) {
    volumeMixers_.clear();
  }
  if (!captureMixers_.isEmpty()) {
    captureMixers_.clear();
  }
  if (!switches_->isEmpty()) {
    switches_->clearAll();
  }
  snd_mixer_t *handle = getMixerHanlde(id_);
  snd_mixer_selem_id_t *smid;
  snd_mixer_selem_id_alloca(&smid);
  QString deviceName;
  snd_mixer_elem_t *element;
  for (element = snd_mixer_first_elem(handle); element;
       element = snd_mixer_elem_next(element)) {
    if (!snd_mixer_elem_empty(element)) {
      switcher sCap;
      snd_mixer_selem_get_id(element, smid);
      deviceName = QString::fromLocal8Bit(snd_mixer_selem_id_get_name(smid));
      snd_mixer_selem_channel_id_t channel = checkMixerChannels(element);
      if (snd_mixer_selem_has_playback_volume(element) ||
          snd_mixer_selem_has_playback_volume_joined(element) ||
          snd_mixer_selem_has_common_volume(element)) {
        volumeMixers_ << deviceName;
      }
      if (snd_mixer_selem_has_capture_volume(element) ||
          snd_mixer_selem_has_capture_volume_joined(element)) {
        captureMixers_ << deviceName;
      }
      if (snd_mixer_selem_has_capture_switch(element) ||
          snd_mixer_selem_has_common_switch(element) ||
          snd_mixer_selem_has_capture_switch_joined(element) ||
          snd_mixer_selem_has_capture_switch_exclusive(element)) {
        int value = 0;
        int res = snd_mixer_selem_get_capture_switch(element, channel, &value);
        checkError(res);
        if (res == 0) {
          sCap = qMakePair(deviceName, bool(value));
          switches_->pushBack(CAPTURE, sCap);
        }
      }
      if (snd_mixer_selem_has_playback_switch(element) ||
          snd_mixer_selem_has_playback_switch_joined(element)) {
        int value = 0;
        int res = snd_mixer_selem_get_playback_switch(element, channel, &value);
        checkError(res);
        if (res == 0) {
          sCap = qMakePair(deviceName, bool(value));
          switches_->pushBack(PLAYBACK, sCap);
        }
      }
      if (snd_mixer_selem_is_enumerated(element)) {
        uint value = 0;
        int res = snd_mixer_selem_get_enum_item(element, channel, &value);
        checkError(res);
        if (res == 0) {
          sCap = qMakePair(deviceName, bool(value));
          switches_->pushBack(ENUM, sCap);
        }
      }
    }
  }
  checkError(snd_mixer_close(handle));
  initMixerList();
}

void AlsaDevice::initMixerList() {
  bool isplay(!volumeMixers_.isEmpty());
  bool isrec(!captureMixers_.isEmpty());
  if (!mixers_.isEmpty()) {
    mixers_.clear();
  }
  if (isplay) {
    for (const QString &mixer : std::as_const(volumeMixers_)) {
      mixers_ << mixer;
    }
  }
  if (isrec && !mixers_.isEmpty()) {
    for (const QString &mixer : std::as_const(captureMixers_)) {
      mixers_ << mixer;
    }
  }
  if (!mixers_.isEmpty()) {
    setCurrentMixer(0);
  }
}

snd_mixer_elem_t *AlsaDevice::initMixerElement(snd_mixer_t *handle,
                                               const char *mixer) {
  snd_mixer_selem_id_t *smid;
  snd_mixer_selem_id_alloca(&smid);
  snd_mixer_selem_id_set_index(smid, 0);
  snd_mixer_selem_id_set_name(smid, mixer);
  snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, smid);
  return elem;
}

snd_mixer_t *AlsaDevice::getMixerHanlde(int id) {
  snd_mixer_t *handle;
  int res = snd_mixer_open(&handle, 0);
  checkError(res);
  if (res == 0) {
    res = snd_mixer_attach(handle, formatCardName(id).toLocal8Bit());
    checkError(res);
    if (res == 0) {
      res = snd_mixer_selem_register(handle, nullptr, nullptr);
      checkError(res);
      if (res == 0) {
        res = snd_mixer_load(handle);
        checkError(res);
      }
    }
  }
  return handle;
}

snd_mixer_selem_channel_id_t
AlsaDevice::checkMixerChannels(snd_mixer_elem_t *element) {
  if (snd_mixer_selem_is_playback_mono(element)) {
    return SND_MIXER_SCHN_MONO;
  } else {
    for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
      if (snd_mixer_selem_has_playback_channel(
              element, snd_mixer_selem_channel_id_t(channel))) {
        return snd_mixer_selem_channel_id_t(channel);
      }
    }
  }
  if (snd_mixer_selem_is_capture_mono(element)) {
    return SND_MIXER_SCHN_MONO;
  } else {
    for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
      if (snd_mixer_selem_has_capture_channel(
              element, snd_mixer_selem_channel_id_t(channel))) {
        return snd_mixer_selem_channel_id_t(channel);
      }
    }
  }
  return SND_MIXER_SCHN_UNKNOWN;
}
// This part of code from alsa-utils.git/alsamixer/volume_mapping.c
// Copyright (c) 2010 Clemens Ladisch <clemens@ladisch.de>
double AlsaDevice::getExp10(double value) { return exp(value * log(10)); }

double AlsaDevice::getNormVolume(snd_mixer_elem_t *element) {
  long min, max, value;
  double norm, minNorm;
  int err;
  snd_mixer_selem_channel_id_t chanelid = checkMixerChannels(element);
  if (snd_mixer_selem_has_playback_volume(element)) {
    err = snd_mixer_selem_get_playback_dB_range(element, &min, &max);
    checkError(err);
    if (err < 0 || min >= max) {
      err = snd_mixer_selem_get_playback_volume_range(element, &min, &max);
      checkError(err);
      if (err < 0 || min == max) {
        return ZERO;
      }
      err = snd_mixer_selem_get_playback_volume(element, chanelid, &value);
      checkError(err);
      if (err < 0) {
        return ZERO;
      }
      return double(value - min) / double(max - min);
    }
    err = snd_mixer_selem_get_playback_dB(element, chanelid, &value);
    checkError(err);
    if (err < 0) {
      return ZERO;
    }
    if (useLinearDb(min, max)) {
      return double(value - min) / double(max - min);
    }
    norm = getExp10(double(value - max) / 6000.0);
    if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
      minNorm = getExp10(double(min - max) / 6000.0);
      norm = (norm - minNorm) / (1 - minNorm);
    }
    return norm;
  } else if (snd_mixer_selem_has_capture_volume(element)) {
    err = snd_mixer_selem_get_capture_dB_range(element, &min, &max);
    checkError(err);
    if (err < 0 || min >= max) {
      err = snd_mixer_selem_get_capture_volume_range(element, &min, &max);
      checkError(err);
      if (err < 0 || min == max) {
        return ZERO;
      }
      err = snd_mixer_selem_get_capture_volume(element, chanelid, &value);
      checkError(err);
      if (err < 0) {
        return ZERO;
      }
      return double(value - min) / double(max - min);
    }

    err = snd_mixer_selem_get_capture_dB(element, chanelid, &value);
    checkError(err);
    if (err < 0) {
      return ZERO;
    }
    if (useLinearDb(min, max)) {
      return double(value - min) / double(max - min);
    }
    norm = getExp10(double(value - max) / 6000.0);
    if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
      minNorm = getExp10(double(min - max) / 6000.0);
      norm = (norm - minNorm) / (1 - minNorm);
    }
    return norm;
  }
  return ZERO;
}

bool AlsaDevice::useLinearDb(long min, long max) {
  const long maxDB(24);
  return (max - min) <= maxDB * 100;
}

void AlsaDevice::setNormVolume(snd_mixer_elem_t *element, double volume) {
  long min, max, value;
  double min_norm;
  int err;
  if (snd_mixer_selem_has_playback_volume(element)) {
    err = snd_mixer_selem_get_playback_dB_range(element, &min, &max);
    checkError(err);
    if (err < 0 || min >= max) {
      err = snd_mixer_selem_get_playback_volume_range(element, &min, &max);
      checkError(err);
      if (err < 0) {
        return;
      }
      value = lrint(volume * double(max - min)) + min;
      checkError(snd_mixer_selem_set_playback_volume_all(element, value));
      return;
    }
    if (useLinearDb(min, max)) {
      value = lrint(volume * double(max - min)) + min;
      checkError(snd_mixer_selem_set_playback_dB_all(element, value, 1));
      return;
    }
    if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
      min_norm = getExp10(double(min - max) / 6000.0);
      volume = volume * (1 - min_norm) + min_norm;
    }
    value = lrint(6000.0 * log10(volume)) + max;
    checkError(snd_mixer_selem_set_playback_dB_all(element, value, 1));
    return;
  } else if (snd_mixer_selem_has_capture_volume(element)) {
    err = snd_mixer_selem_get_capture_dB_range(element, &min, &max);
    checkError(err);
    if (err < 0 || min >= max) {
      err = snd_mixer_selem_get_capture_volume_range(element, &min, &max);
      checkError(err);
      if (err < 0) {
        return;
      }
      value = lrint(volume * double(max - min)) + min;
      checkError(snd_mixer_selem_set_capture_volume_all(element, value));
      return;
    }
    if (useLinearDb(min, max)) {
      value = lrint(volume * double(max - min)) + min;
      checkError(snd_mixer_selem_set_capture_dB_all(element, value, 1));
      return;
    }
    if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
      min_norm = getExp10(double(min - max) / 6000.0);
      volume = volume * (1 - min_norm) + min_norm;
    }
    value = lrint(6000.0 * log10(volume)) + max;
    checkError(snd_mixer_selem_set_capture_dB_all(element, value, 1));
  }
}

// This part of code from alsa-utils.git/alsamixer/volume_mapping.c

void AlsaDevice::setDeviceVolume(double volume) {
  if (!currentMixerName_.isEmpty() && !mixers_.isEmpty()) {
    snd_mixer_t *handle = getMixerHanlde(id_);
    snd_mixer_elem_t *element =
        initMixerElement(handle, currentMixerName_.toLocal8Bit());
    if (!snd_mixer_elem_empty(element)) {
      setNormVolume(element, volume / 100);
    }
    checkError(snd_mixer_close(handle));
  } else {
    checkError(ERROR_TITLE, "setDeviceVolume::current mixer is empty");
  }
}

double AlsaDevice::getVolume() {
  double volume(ZERO);
  if (!currentMixerName_.isEmpty() && !mixers_.isEmpty()) {
    snd_mixer_t *handle = getMixerHanlde(id_);
    snd_mixer_elem_t *elem =
        initMixerElement(handle, currentMixerName_.toLocal8Bit());
    if (!snd_mixer_elem_empty(elem)) {
      volume = getNormVolume(elem) * 100;
    }
    checkError(snd_mixer_close(handle));
  } else {
    checkError(ERROR_TITLE, "getVolume::current mixer is empty");
  }
  return volume;
}

void AlsaDevice::setSwitch(const QString &mixer, int id, bool enabled) const {
  snd_mixer_t *handle = getMixerHanlde(id_);
  snd_mixer_elem_t *elem = initMixerElement(handle, mixer.toLocal8Bit());
  if (!snd_mixer_elem_empty(elem)) {
    switch (id) {
    case PLAYBACK:
    default:
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
  }
  checkError(snd_mixer_close(handle));
}

void AlsaDevice::setMute(bool enabled) {
  if (!currentMixerName_.isEmpty()) {
    snd_mixer_t *handle = getMixerHanlde(id_);
    snd_mixer_elem_t *elem =
        initMixerElement(handle, currentMixerName_.toLocal8Bit());
    if (!snd_mixer_elem_empty(elem)) {
      if (snd_mixer_selem_has_playback_switch(elem) ||
          snd_mixer_selem_has_playback_switch_joined(elem)) {
        checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
      }
      if (snd_mixer_selem_has_capture_switch(elem) ||
          snd_mixer_selem_has_common_switch(elem) ||
          snd_mixer_selem_has_capture_switch_joined(elem) ||
          snd_mixer_selem_has_capture_switch_exclusive(elem)) {
        checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
      }
    }
    checkError(snd_mixer_close(handle));
  } else {
    checkError(ERROR_TITLE, "setMute::current mixer is empty");
  }
}

bool AlsaDevice::getMute() {
  if (!currentMixerName_.isEmpty() && !mixers_.isEmpty()) {
    snd_mixer_t *handle = getMixerHanlde(id_);
    snd_mixer_elem_t *elem =
        initMixerElement(handle, currentMixerName_.toLocal8Bit());
    if (!snd_mixer_elem_empty(elem)) {
      snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);
      if (snd_mixer_selem_has_playback_switch(elem) ||
          snd_mixer_selem_has_playback_switch_joined(elem)) {
        int value = 0;
        checkError(snd_mixer_selem_get_playback_switch(elem, channel, &value));
        checkError(snd_mixer_close(handle));
        return bool(value);
      }
      if (snd_mixer_selem_has_capture_switch(elem) ||
          snd_mixer_selem_has_common_switch(elem) ||
          snd_mixer_selem_has_capture_switch_joined(elem) ||
          snd_mixer_selem_has_capture_switch_exclusive(elem)) {
        int value = 0;
        checkError(snd_mixer_selem_get_capture_switch(elem, channel, &value));
        checkError(snd_mixer_close(handle));
        return bool(value);
      }
    }
    checkError(snd_mixer_close(handle));
  } else {
    checkError(ERROR_TITLE, "getMute::current mixer is empty");
  }
  return true;
}

QString AlsaDevice::formatCardName(int id) {
  return QString("hw:%1").arg(QString::number(id));
}

void AlsaDevice::setCurrentMixer(int id) {
  if (id >= 0 && id < mixers_.size()) {
    currentMixerId_ = id;
    currentMixerName_ = mixers_.at(id);
  }
}

const QString &AlsaDevice::name() const { return name_; }

int AlsaDevice::id() const { return id_; }

const QStringList &AlsaDevice::mixers() const { return mixers_; }

void AlsaDevice::checkError(int errorIndex) {
  if (errorIndex < 0) {
#ifdef ISDEBUG
    qDebug() << "Error index = " << errorIndex;
    qDebug() << QString::fromLocal8Bit(snd_strerror(errorIndex));
#endif
    QMessageBox::critical(nullptr, ERROR_TITLE,
                          QString::fromLocal8Bit(snd_strerror(errorIndex)));
  }
}

void AlsaDevice::checkError(const QString &title, const QString &message) {
  if (!title.isEmpty() && !message.isEmpty()) {
    QMessageBox::critical(nullptr, title, message);
  }
}

bool AlsaDevice::haveMixers() { return !mixers_.isEmpty(); }

MixerSwitches::Ptr AlsaDevice::switches() {
  updateElements();
  return switches_;
}

AlsaDevice::AlsaDevice(AlsaDevice const &ad)
    : id_(ad.id()), currentMixerId_(ad.currentMixerId_) {}
