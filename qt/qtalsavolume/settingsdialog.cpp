/*
 * settingsgialog.cpp
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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtGui>
#include <QListWidget>
#ifdef ISDEBUG
#include <QDebug>
#endif

SettingsDialog::SettingsDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::SettingsDialog),
  soundCards_(QStringList()),
  mixers_(QStringList()),
  isAutorun_(false),
  itemsAdded_(false),
  playbacks_(new QListWidget(this)),
  captures_(new QListWidget(this)),
  enums_(new QListWidget(this)),
  l1_(new QLabel(tr("Playback Switches"),this)),
  l2_(new QLabel(tr("Capture Switches"),this)),
  l3_(new QLabel(tr("Enum Switches"),this))
{
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onCancel()));
	playbacks_->setToolTip(PLAYBACK_TIP);
	captures_->setToolTip(CAPTURE_TIP);
	enums_->setToolTip(ENUMS_TIP);
	ui->verticalLayout_3->addWidget(l1_);
	ui->verticalLayout_3->addWidget(playbacks_);
	ui->verticalLayout_3->addWidget(l2_);
	ui->verticalLayout_3->addWidget(captures_);
	ui->verticalLayout_3->addWidget(l3_);
	ui->verticalLayout_3->addWidget(enums_);
	ui->tabWidget->setCurrentIndex(0);
#ifndef USE_PULSE
	ui->usePulseaudio->setEnabled(false);
#endif
}

SettingsDialog::~SettingsDialog()
{
	delete l3_;
	delete l2_;
	delete l1_;
	delete enums_;
	delete captures_;
	delete playbacks_;
	delete ui;
}

void SettingsDialog::setSoundCards(const QStringList &cards)
{
	ui->cardBox->blockSignals(true);
	soundCards_ = cards;
	if (ui->cardBox->count() > 0) {
		ui->cardBox->clear();
	}
	ui->cardBox->addItems(soundCards_);
	ui->cardBox->blockSignals(false);
}

void SettingsDialog::setMixers(const QStringList &mixers)
{
	ui->mixerBox->blockSignals(true);
	mixers_ = mixers;
	if (ui->mixerBox->count() > 0) {
		ui->mixerBox->clear();
	}
	if (mixers_.size() > 0) {
		ui->mixerBox->addItems(mixers_);
	}
	ui->mixerBox->blockSignals(false);
}

void SettingsDialog::connectSignals()
{
	connect(playbacks_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onPBAction(QListWidgetItem*)));
	connect(captures_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onCPAction(QListWidgetItem*)));
	connect(enums_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onENAction(QListWidgetItem*)));
	connect(ui->darkRadio, SIGNAL(toggled(bool)), this, SLOT(onDarkStyle(bool)));
	connect(ui->lightRadio, SIGNAL(toggled(bool)), this, SLOT(onLightStyle(bool)));
	connect(ui->cardBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSoundCard(int)));
	connect(ui->mixerBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onMixer(QString)));
	connect(ui->isAutorun, SIGNAL(toggled(bool)), this, SLOT(onAutorun(bool)));
#ifdef USE_PULSE
	connect(ui->usePulseaudio, SIGNAL(toggled(bool)), this, SLOT(onPulseSoundSystem(bool)));
#endif
}

void SettingsDialog::onSoundCard(int changed)
{
	emit soundCardChanged(changed);
}

void SettingsDialog::onMixer(const QString &changed)
{
	emit mixerChanged(changed);
}

void SettingsDialog::setCurrentCard(int index)
{
	if (index < soundCards_.size()) {
		ui->cardBox->setCurrentIndex(index);
	}
	else {
		ui->cardBox->setCurrentIndex(0);
	}
}

void SettingsDialog::setCurrentMixer(const QString &mixer)
{
	if (mixers_.contains(mixer)) {
		ui->mixerBox->setCurrentIndex(mixers_.indexOf(mixer));
	}
}

void SettingsDialog::onOk()
{
	const QString mixer = ui->mixerBox->currentText();
	const int card = ui->cardBox->currentIndex();
	emit soundCardChanged(card);
	emit mixerChanged(mixer);
	hide();
}

void SettingsDialog::onCancel()
{
	hide();
}

void SettingsDialog::onAutorun(bool toggle)
{
	if (itemsAdded_) {
		emit autorunChanged(toggle);
	}
}

void SettingsDialog::closeEvent(QCloseEvent *)
{
	hide();
}

void SettingsDialog::setPlaybackChecks(const QList<switcher> &pbList)
{
	itemsAdded_ = false;
	if (playbacks_->count() > 0) {
		playbacks_->clear();
	}
	foreach (switcher item, pbList) {
		QListWidgetItem *cb = new QListWidgetItem(item.first, playbacks_);
		cb->setFlags(cb->flags() | Qt::ItemIsUserCheckable);
		if (item.second) {
			cb->setCheckState(Qt::Checked);
		}
		else {
			cb->setCheckState(Qt::Unchecked);
		}
		playbacks_->addItem(cb);
	}
	playbacks_->show();
	itemsAdded_ = true;
}

void SettingsDialog::setCaptureChecks(const QList<switcher> &cList)
{
	itemsAdded_ = false;
	if (captures_->count() > 0 ){
		captures_->clear();
	}
	foreach (switcher item, cList) {
		QListWidgetItem *cb = new QListWidgetItem(item.first, captures_);
		cb->setFlags(cb->flags() | Qt::ItemIsUserCheckable);
		if (item.second) {
			cb->setCheckState(Qt::Checked);
		}
		else {
			cb->setCheckState(Qt::Unchecked);
		}
		captures_->addItem(cb);
	}
	captures_->show();
	itemsAdded_ = true;
}

void SettingsDialog::setEnumChecks(const QList<switcher> &eList)
{
	itemsAdded_ = false;
	if (enums_->count() > 0 ){
		enums_->clear();
	}
	foreach (switcher item, eList) {
		QListWidgetItem *cb = new QListWidgetItem(item.first, enums_);
		cb->setFlags(cb->flags() | Qt::ItemIsUserCheckable);
		if (item.second) {
			cb->setCheckState(Qt::Checked);
		}
		else {
			cb->setCheckState(Qt::Unchecked);
		}
		enums_->addItem(cb);
	}
	enums_->show();
	itemsAdded_ = true;
}

void SettingsDialog::setAutorun(bool isAutorun)
{
	itemsAdded_ = false;
	ui->isAutorun->setChecked(isAutorun);
	itemsAdded_ = true;
}

void SettingsDialog::onPBAction(QListWidgetItem *item)
{
	if (itemsAdded_) {
		const QString name = item->text();
		const Qt::CheckState checked = item->checkState();
		emit playChanged(name, (checked == Qt::Checked));
	}
}

void SettingsDialog::onCPAction(QListWidgetItem *item)
{
	if (itemsAdded_) {
		const QString name = item->text();
		const Qt::CheckState checked = item->checkState();
		emit captChanged(name, (checked == Qt::Checked));
	}
}

void SettingsDialog::onENAction(QListWidgetItem *item)
{
	if (itemsAdded_) {
		const QString name = item->text();
		const Qt::CheckState checked = item->checkState();
		emit enumChanged(name, (checked == Qt::Checked));
	}
}

void SettingsDialog::onDarkStyle(bool toggled)
{
	if (toggled) {
		emit styleChanged(false);
	}
}

void SettingsDialog::onLightStyle(bool toggled)
{
	if (toggled) {
		emit styleChanged(true);
	}
}

void SettingsDialog::setIconStyle(bool isLight)
{
	if (isLight) {
		ui->lightRadio->setChecked(true);
	}
	else {
		ui->darkRadio->setChecked(true);
	}
}

void SettingsDialog::onPulseSoundSystem(bool toggled)
{
	emit soundSystemChanged(toggled);
}

void SettingsDialog::setUsePulse(bool isPulse)
{
#ifdef USE_PULSE
	ui->usePulseaudio->setChecked(isPulse);
#else
	Q_UNUSED(isPulse)
#endif
}

void SettingsDialog::hideAlsaElements(bool isHide)
{
	ui->mixerBox->setEnabled(!isHide);
	playbacks_->setEnabled(!isHide);
	captures_->setEnabled(!isHide);
	enums_->setEnabled(!isHide);
}
