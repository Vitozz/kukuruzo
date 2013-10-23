#include "popupwindow.h"
#include <QApplication>
#include <QtGui>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("thesomeprojects");
	a.setApplicationName("qtalsavolume");
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("System tray not detected"));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);
	PopupWindow w;
	w.show();
	w.hide();
	return a.exec();
}
