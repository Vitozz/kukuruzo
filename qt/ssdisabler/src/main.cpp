#include "ssdisabler.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>
#include <QDir>
#include <QLocale>

#define APP_ORG "thesomeprojects"
#define APP_VERSION "0.0.1"
#define APP_NAME "ssdisable"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName(APP_ORG);
	a.setApplicationVersion(APP_VERSION);
	a.setApplicationName(APP_NAME);
	QTranslator translator;
	QStringList localeDirs;
	localeDirs << QString(QDir::currentPath());
	localeDirs << QString("%1/langs").arg(QDir::currentPath());
	localeDirs << QString("../share/%1/langs").arg(APP_NAME);
	localeDirs << QString(qApp->applicationDirPath() + "/langs");
	localeDirs << QString("/usr/share/ssdisable/langs");
	localeDirs << QString("/usr/local/share/ssdisable/langs");
	localeDirs << QString(QDir::home().absolutePath() + "/.local/share/ssdisable/langs");
	QString langFile = qApp->applicationName();
	foreach(const QString &dir, localeDirs){
		if (translator.load(QLocale::system(),langFile, "_", dir )) {
			qApp->installTranslator(&translator);
			break;
		}
	}
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("System tray not detected"));
		return 1;
	}
	SSDisabler w;
	w.show();
	w.hide();
	QSharedMemory sharedMemory;
	sharedMemory.setKey("SSDisabler");
	if (sharedMemory.attach()) {
		return 0;
	}
	if (!sharedMemory.create(1)) {
		return 0;
	}
	else{
		return a.exec();
	}
}
