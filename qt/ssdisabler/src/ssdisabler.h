#ifndef SSDISABLER_H
#define SSDISABLER_H

#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>

class SSDisabler : public QWidget
{
	Q_OBJECT

public:
	SSDisabler(QWidget *parent = 0);
	~SSDisabler();

protected slots:
	void onClick_();
	void onExit_();
	void iconActivated_(QSystemTrayIcon::ActivationReason reason);

private:
	void createActions();
	void createTrayMenu();
	void changeTrayIcon();
	void switchScreensaver();
	void suspend();
	void resume();
	void startProcess(const QStringList &args);

private:
	bool suspended_;
	QAction *click_;
	QAction *exit_;
	QMenu *trayMenu_;
	QSystemTrayIcon *trayIcon_;
	QString winId_;
};

#endif // SSDISABLER_H
