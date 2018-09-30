#-------------------------------------------------
#
# Project created by QtCreator 2014-03-03T02:38:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ssdisabler
TEMPLATE = app


SOURCES += main.cpp\
        ssdisabler.cpp

HEADERS  += ssdisabler.h

DATADIR = $$PREFIX/share/ssdisabler

TRANSLATIONS = $$TARGET_en.ts \
	       $$TARGET_ru.ts \
	       $$TARGET_uk.ts


RESOURCES += \
	     ssdisable.qrc

unix{
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files =../ssdisabler.desktop
	translations.path = $$DATADIR/languages
	translations.extra = lrelease SSDisabler.pro && cp -f *.qm  $(INSTALL_ROOT)$$translations.path
	icon1.path = $$DATADIR/icons
	icon1.files = \
		     icon.png

	INSTALLS += dt \
		    translations \
		    icon1
}

