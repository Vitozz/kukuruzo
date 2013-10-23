/*
 * settingsgialog.cpp
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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtGui>

SettingsDialog::SettingsDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onCancel()));
	playbacks_ = new QListWidget(this);
	captures_ = new QListWidget(this);
	enums_ = new QListWidget(this);
	QLabel *l1 = new QLabel(tr("Playback Switches"),this);
	QLabel *l2 = new QLabel(tr("Capture Switches"),this);
	QLabel *l3 = new QLabel(tr("Enum Switches"),this);
	ui->verticalLayout_3->addWidget(l1);
	ui->verticalLayout_3->addWidget(playbacks_);
	ui->verticalLayout_3->addWidget(l2);
	ui->verticalLayout_3->addWidget(captures_);
	ui->verticalLayout_3->addWidget(l3);
	ui->verticalLayout_3->addWidget(enums_);
	ui->tabWidget->setCurrentIndex(0);
	itemsAdded_ = false;
	connect(playbacks_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onPBAction(QListWidgetItem*)));
	connect(captures_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onCPAction(QListWidgetItem*)));
	connect(enums_, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onENAction(QListWidgetItem*)));
}

SettingsDialog::~SettingsDialog()
{
	delete playbacks_;
	delete captures_;
	delete enums_;
	delete ui;
}

void SettingsDialog::setSoundCards(QStringList cards)
{
	soundCards_ = cards;
	if (ui->cardBox->count() > 0) {
		ui->cardBox->clear();
	}
	ui->cardBox->addItems(soundCards_);
	ui->cardBox->setCurrentIndex(0);
}

void SettingsDialog::setMixers(QStringList mixers)
{
	mixers_ = mixers;
	if (ui->mixerBox->count() > 0) {
		ui->mixerBox->clear();
	}
	if (mixers_.size() > 0) {
		ui->mixerBox->addItems(mixers_);
	}
}

void SettingsDialog::connectSignals()
{
	connect(ui->cardBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSoundCard(QString)));
	connect(ui->mixerBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onMixer(QString)));
	connect(ui->isAutorun, SIGNAL(toggled(bool)), this, SLOT(onAutorun(bool)));
}

void SettingsDialog::onSoundCard(const QString &changed)
{
	emit soundCardChanged(changed);
}

void SettingsDialog::onMixer(const QString &changed)
{
	emit mixerChanged(changed);
}

void SettingsDialog::setCurrentCard(const QString &card)
{
	if (soundCards_.contains(card)) {
		ui->cardBox->setCurrentIndex(soundCards_.indexOf(card));
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
	QString mixer = ui->mixerBox->currentText();
	QString card = ui->cardBox->currentText();
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

void SettingsDialog::setPlaybackChecks(QList<switcher> pbList)
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

void SettingsDialog::setCaptureChecks(QList<switcher> cList)
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

void SettingsDialog::setEnumChecks(QList<switcher> eList)
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
		QString name = item->text();
		Qt::CheckState checked = item->checkState();
		emit playChanged(name, (checked == Qt::Checked));
	}
}

void SettingsDialog::onCPAction(QListWidgetItem *item)
{
	if (itemsAdded_) {
		QString name = item->text();
		Qt::CheckState checked = item->checkState();
		emit captChanged(name, (checked == Qt::Checked));
	}
}

void SettingsDialog::onENAction(QListWidgetItem *item)
{
	if (itemsAdded_) {
		QString name = item->text();
		Qt::CheckState checked = item->checkState();
		emit enumChanged(name, (checked == Qt::Checked));
	}
}
