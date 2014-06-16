/*
 * Copyright (C) 2014 Vitaly Tonkacheyev <thetvg@gmail.com>
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
 */

#include "pulsework.h"
#include "defines.h"

PulseWork::PulseWork()
: pulse_(new PulseCore(APP_NAME)),
  sinks_(pulse_->getSinksDescriptions()),
  sources_(pulse_->getSourcesDescriptions())
{
}

PulseWork::~PulseWork()
{
	delete pulse_;
}

QStringList PulseWork::getCardList() const
{
	QStringList cards;
	foreach (const QString &card, sinks_) {
		cards << card;
	}
	foreach (const QString &card, sources_) {
		cards << card;
	}
	return cards;
}

QString PulseWork::getCardName(const QString &description) const
{
	return pulse_->getDeviceName(description);
}

QString PulseWork::getCardDescription(const QString &name) const
{
	return pulse_->getDeviceDescription(name);
}

int PulseWork::getVolume(const QString &description) const
{
	return pulse_->getVolume(description);
}

bool PulseWork::getMute(const QString &description) const
{
	return pulse_->getMute(description);
}

void PulseWork::setMute(bool mute)
{
	pulse_->setMute(currentCard_, mute);
}

void PulseWork::setVolume(int value)
{
	pulse_->setVolume(currentCard_, value);
}

QString PulseWork::getDefaultCard() const
{
	return pulse_->defaultSink();
}

void PulseWork::setCurrentCard(const QString &description)
{
	currentCard_ = description;
}
