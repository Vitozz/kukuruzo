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
	localeDirs << QString("%1/languages").arg(QDir::currentPath());
	localeDirs << QString(qApp->applicationDirPath() + "/languages");
	localeDirs << QString("/usr/share/ssdisable/languages");
	localeDirs << QString("/usr/local/share/ssdisable/languages");
	localeDirs << QString(QDir::home().absolutePath() + "/.local/share/ssdisable/languages");
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
