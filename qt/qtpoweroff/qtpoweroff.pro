#-------------------------------------------------
#
# Project created by QtCreator 2014-11-16T06:28:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): {
	QT += widgets
	DEFINES += HAVE_QT5
}

CONFIG(debug, debug|release) {
	greaterThan(QT_MAJOR_VERSION, 4):win32: CONFIG += console
	DEFINES += IS_DEBUG
}

unix:!macx {
	DEFINES += HAVE_DBUS
	QT += dbus
}

TARGET = qtpoweroff
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp

HEADERS  += mainwindow.h \
	defines.h

FORMS    += mainwindow.ui

RESOURCES += \
	qtpoweroff.qrc

PREFIX = /usr

DATADIR = $$PREFIX/share/qtpoweroff

LANG_PATH = languages
TRANSLATIONS = $$LANG_PATH/qtpoweroff_en.ts \
	       $$LANG_PATH/qtpoweroff_ru.ts \
	       $$LANG_PATH/qtpoweroff_uk.ts
unix{
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files = qtpoweroff.desktop
	translations.path = $$DATADIR/languages
	translations.extra = lrelease qtpoweroff.pro && cp -f $$LANG_PATH/qtpoweroff_*.qm  $(INSTALL_ROOT)$$translations.path
	icon1.path = $$DATADIR/icons
	icon1.files = \
		     images/tb_icon.png

	INSTALLS += dt \
		    translations \
		    icon1
}
