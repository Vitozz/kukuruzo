#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QLocale>
#include "mainwindow.h"

#define ORGANIZATION "thesomeprojects"
#define APPNAME "regexptest"
#define APPVERISON "0.3.4"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName(QString(ORGANIZATION));
	a.setApplicationName(QString(APPNAME));
	a.setApplicationVersion(QString(APPVERISON));
	QTranslator translator;
	QStringList localeDirs;
	localeDirs << QString("%1/langs").arg(QDir::currentPath());
	localeDirs << QString(qApp->applicationDirPath() + "/langs");
	localeDirs << QString("../share/%1/langs").arg(APPNAME);
	localeDirs << QString("/usr/share/%1/langs").arg(QString(APPNAME));
	localeDirs << QString("/usr/local/share/%1/langs").arg(QString(APPNAME));
	localeDirs << QString(QDir::home().absolutePath() + "/.local/share/%1/langs").arg(QString(APPNAME));
	QString langFile = QString(APPNAME);
	foreach(const QString &dir, localeDirs){
		if (translator.load(QLocale::system(),langFile, "_", dir )) {
			qApp->installTranslator(&translator);
			break;
		}
	}
	MainWindow w;
	w.show();
	return a.exec();
}
