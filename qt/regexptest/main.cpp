#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QLocale>
#include "mainwindow.h"

#define ORGANIZATION "thesomeprojects"
#define APPNAME "regexptest"
#define APPVERISON "0.3.6"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName(QString(ORGANIZATION));
	a.setApplicationName(QString(APPNAME));
	a.setApplicationVersion(QString(APPVERISON));
	QTranslator translator;
	const QStringList localeDirs = QStringList()
				       << QString("%1/langs").arg(QDir::currentPath())
				       << QString("%1/langs").arg(qApp->applicationDirPath())
				       << QString("../share/%1/langs").arg(APPNAME)
				       << QString("/usr/share/%1/langs").arg(QString(APPNAME))
				       << QString("/usr/local/share/%1/langs").arg(QString(APPNAME))
				       << QString("%1/.local/share/%2/langs").arg(QDir::home().absolutePath(),QString(APPNAME));
	const QString langFile(APPNAME);
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
