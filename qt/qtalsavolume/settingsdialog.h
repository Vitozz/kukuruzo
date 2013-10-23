#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

typedef QPair<QString, bool> switcher;

class QListWidget;
class QListWidgetItem;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();
	void setSoundCards(QStringList cards);
	void setMixers(QStringList mixers);
	void setAutorun(bool isAutorun);
	void connectSignals();
	void setCurrentCard(const QString &card);
	void setCurrentMixer(const QString &mixer);
	void setPlaybackChecks(QList<switcher> pbList);
	void setCaptureChecks(QList<switcher> cList);
	void setEnumChecks(QList<switcher> eList);

protected:
	void closeEvent(QCloseEvent *);

protected slots:
	void onSoundCard(const QString &changed);
	void onMixer(const QString &changed);
	void onAutorun(bool toggle);
	void onOk();
	void onCancel();
	void onPBAction(QListWidgetItem *item);
	void onCPAction(QListWidgetItem *item);
	void onENAction(QListWidgetItem *item);
signals:
	void soundCardChanged(const QString &card);
	void mixerChanged(const QString &mixer);
	void autorunChanged(bool isIt);
	void playChanged(const QString &check, bool isIt);
	void captChanged(const QString &check, bool isIt);
	void enumChanged(const QString &check, bool isIt);

private:
	Ui::SettingsDialog *ui;
	QStringList soundCards_;
	QStringList mixers_;
	bool isAutorun_;
	QListWidget *playbacks_;
	QListWidget *captures_;
	QListWidget *enums_;
};

#endif // SETTINGSDIALOG_H
