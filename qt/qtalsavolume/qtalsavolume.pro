#-------------------------------------------------
#
# Project created by QtCreator 2013-10-22T17:06:27
#
#-------------------------------------------------
include(conf.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtalsavolume
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += alsa

SOURCES += main.cpp\
	   popupwindow.cpp \
	   alsawork/alsawork.cpp \
	   alsawork/mixerswitches.cpp \
	   alsawork/volumemixers.cpp \
	   settingsdialog.cpp

HEADERS  += popupwindow.h \
	    alsawork/alsawork.h \
	    alsawork/mixerswitches.h \
	    alsawork/volumemixers.h \
	    settingsdialog.h

FORMS    += \
	    settingsdialog.ui

RESOURCES += \
	    icons.qrc

DATADIR = $$PREFIX/share/qtalsavolume

LANG_PATH = languages
TRANSLATIONS = $$LANG_PATH/qtalsavolume_en.ts \
	       $$LANG_PATH/qtalsavolume_ru.ts \
	       $$LANG_PATH/qtalsavolume_uk.ts

unix{
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files = qtalsavolume.desktop
	res.path = $$DATADIR/
	res.files = icons.qrc
	translations.path = $$DATADIR/languages
	translations.extra = lrelease qtalvavolume.pro && cp -f $$LANG_PATH/qtalsavolume_*.qm  $(INSTALL_ROOT)$$translations.path
	icon.path = $$DATADIR/icons/
	icon.files = icons/tb_icon0.png \
		     icons/tb_icon20.png \
		     icons/tb_icon40.png \
		     icons/tb_icon60.png \
		     icons/tb_icon80.png \
		     icons/tb_icon100.png \
		     icons/volume_ico.png
	INSTALLS += dt \
		    res \
		    translations \
		    icon
}
