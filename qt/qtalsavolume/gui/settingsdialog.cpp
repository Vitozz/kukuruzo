/*
 * settingsgialog.cpp
 * Copyright (C) 2013-2019 Vitaly Tonkacheyev
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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtGui>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#ifdef ISDEBUG
#include <QDebug>
#endif

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::SettingsDialog),
      soundCards_(QStringList()),
      mixers_(QStringList()),
      isAutorun_(false),
      pulseAvailable_(false),
      playbacks_(new QListWidget(this)),
      captures_(new QListWidget(this)),
      enums_(new QListWidget(this)),
      l1_(new QLabel(tr("Playback Switches"),this)),
      l2_(new QLabel(tr("Capture Switches"),this)),
      l3_(new QLabel(tr("Enum Switches"),this))
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onOk);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this](){hide();});
    playbacks_->setToolTip(tr("Enable/Disable Alsa's Playback Switch"));
    captures_->setToolTip(tr("Enable/Disable Alsa's Capture Switch"));
    enums_->setToolTip(tr("Enable/Disable Alsa's Enumerated Switch"));
    ui->verticalLayout_3->addWidget(l1_);
    ui->verticalLayout_3->addWidget(playbacks_);
    ui->verticalLayout_3->addWidget(l2_);
    ui->verticalLayout_3->addWidget(captures_);
    ui->verticalLayout_3->addWidget(l3_);
    ui->verticalLayout_3->addWidget(enums_);
    ui->tabWidget->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
    disconnectSignals();
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
    if (!mixers_.empty()) {
        ui->mixerBox->addItems(mixers_);
    }
    ui->mixerBox->blockSignals(false);
}

void SettingsDialog::connectSignals()
{
    connect(playbacks_, &QListWidget::itemChanged, this, &SettingsDialog::onPBAction);
    connect(captures_, &QListWidget::itemChanged, this, &SettingsDialog::onCPAction);
    connect(enums_, &QListWidget::itemChanged, this, &SettingsDialog::onENAction);
    connect(ui->darkRadio, &QRadioButton::toggled, this, &SettingsDialog::onDarkStyle);
    connect(ui->lightRadio, &QRadioButton::toggled, this, &SettingsDialog::onLightStyle);
    connect(ui->cardBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        emit soundCardChanged(index);
    });
    connect(ui->mixerBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        if(index <= ui->mixerBox->count()) {
            emit mixerChanged(ui->mixerBox->itemText(index));
        }
    });
    connect(ui->isAutorun, &QCheckBox::toggled, this, [this](bool toggle){emit autorunChanged(toggle);});
    connect(ui->usePulseaudio, &QCheckBox::toggled, this, &SettingsDialog::onPulseSoundSystem);
    connect(ui->enableTimer, &QCheckBox::toggled, this, &SettingsDialog::onEnableTimer);
}

void SettingsDialog::disconnectSignals()
{
    playbacks_->disconnect();
    captures_->disconnect();
    enums_->disconnect();
    ui->darkRadio->disconnect();
    ui->lightRadio->disconnect();
    ui->cardBox->disconnect();
    ui->mixerBox->disconnect();
    ui->isAutorun->disconnect();
    ui->usePulseaudio->disconnect();
    ui->enableTimer->disconnect();
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
    const QString mixer(ui->mixerBox->currentText());
    const int card = ui->cardBox->currentIndex();
    emit soundCardChanged(card);
    emit mixerChanged(mixer);
    hide();
}

void SettingsDialog::closeEvent(QCloseEvent *)
{
    hide();
}

void SettingsDialog::setPlaybackChecks(const QList<switcher> &pbList)
{
    if (playbacks_->count() > 0) {
        playbacks_->clear();
    }
    foreach (switcher item, pbList) {
        auto *cb = new QListWidgetItem(item.first, playbacks_);
        cb->setFlags(cb->flags() | Qt::ItemIsUserCheckable);
        if (item.second) {
            cb->setCheckState(Qt::Checked);
        }
        else {
            cb->setCheckState(Qt::Unchecked);
        }
        playbacks_->addItem(cb);
    }
    bool hasItems(playbacks_->count() > 0);
    playbacks_->show();
    playbacks_->setVisible(hasItems);
    l1_->setVisible(hasItems);
}

void SettingsDialog::setCaptureChecks(const QList<switcher> &cList)
{
    if (captures_->count() > 0 ){
        captures_->clear();
    }
    foreach (switcher item, cList) {
        auto *cb = new QListWidgetItem(item.first, captures_);
        cb->setFlags(cb->flags() | Qt::ItemIsUserCheckable);
        if (item.second) {
            cb->setCheckState(Qt::Checked);
        }
        else {
            cb->setCheckState(Qt::Unchecked);
        }
        captures_->addItem(cb);
    }
    bool hasItems(captures_->count() > 0);
    captures_->show();
    captures_->setVisible(hasItems);
    l2_->setVisible(hasItems);
}

void SettingsDialog::setEnumChecks(const QList<switcher> &eList)
{
    if (enums_->count() > 0 ){
        enums_->clear();
    }
    foreach (switcher item, eList) {
        auto *cb = new QListWidgetItem(item.first, enums_);
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
    bool hasItems(enums_->count() > 0);
    enums_->setVisible(hasItems);
    l3_->setVisible(hasItems);
}

void SettingsDialog::setAutorun(bool isAutorun)
{
    ui->isAutorun->setChecked(isAutorun);
}

void SettingsDialog::onPBAction(QListWidgetItem *item)
{
    const Qt::CheckState checked(item->checkState());
    emit playChanged(item->text(), (checked == Qt::Checked));
}

void SettingsDialog::onCPAction(QListWidgetItem *item)
{
    const Qt::CheckState checked = item->checkState();
    emit captChanged(item->text(), (checked == Qt::Checked));
}

void SettingsDialog::onENAction(QListWidgetItem *item)
{
    const Qt::CheckState checked = item->checkState();
    emit enumChanged(item->text(), (checked == Qt::Checked));
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
    ui->usePulseaudio->setChecked(isPulse);
}

void SettingsDialog::hideAlsaElements(bool isHide)
{
    ui->mixerBox->setEnabled(!isHide);
}

void SettingsDialog::setPulseAvailable(bool available)
{
    pulseAvailable_ = available;
    if (!pulseAvailable_) {
        ui->usePulseaudio->setVisible(false);
        ui->usePulseaudio->setEnabled(false);
    }
    else {
        ui->usePulseaudio->setVisible(true);
        ui->usePulseaudio->setEnabled(true);
    }

}

void SettingsDialog::setUsePolling(bool isPoll)
{
    ui->enableTimer->setChecked(isPoll);
}

void SettingsDialog::onEnableTimer(bool toggled)
{
    emit timerEnabled(toggled);
}
