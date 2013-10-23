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
	ui->verticalLayout_3->addWidget(playbacks_);
	ui->verticalLayout_3->addWidget(captures_);
	ui->verticalLayout_3->addWidget(enums_);
	ui->tabWidget->setCurrentIndex(0);
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
	emit autorunChanged(toggle);
}

void SettingsDialog::closeEvent(QCloseEvent *)
{
	hide();
}

void SettingsDialog::setPlaybackChecks(QList<switcher> pbList)
{
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
}

void SettingsDialog::setCaptureChecks(QList<switcher> cList)
{
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
}

void SettingsDialog::setEnumChecks(QList<switcher> eList)
{
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
}

void SettingsDialog::onPBAction(QListWidgetItem *item)
{
	QString name = item->text();
	Qt::CheckState checked = item->checkState();
	emit playChanged(name, (checked == Qt::Checked));
}

void SettingsDialog::onCPAction(QListWidgetItem *item)
{
	QString name = item->text();
	Qt::CheckState checked = item->checkState();
	emit captChanged(name, (checked == Qt::Checked));
}

void SettingsDialog::onENAction(QListWidgetItem *item)
{
	QString name = item->text();
	Qt::CheckState checked = item->checkState();
	emit enumChanged(name, (checked == Qt::Checked));
}
