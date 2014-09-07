#-------------------------------------------------
#
# Project created by QtCreator 2013-10-22T17:06:27
#
#-------------------------------------------------
include(conf.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4){
	QT += widgets gui
	DEFINES += HAVE_QT5
}

TARGET = qtalsavolume
TEMPLATE = app

#CONFIG += pulseaudio

CONFIG += link_pkgconfig
PKGCONFIG += alsa

pulseaudio {
	DEFINES += USE_PULSE
	PKGCONFIG += libpulse
	SOURCES += pulsework/pulsecore.cpp \
		   pulsework/pulsedevice.cpp
	HEADERS += pulsework/pulsecore.h \
		   pulsework/pulsedevice.h
}

SOURCES += main.cpp\
	   popupwindow.cpp \
	   settingsdialog.cpp \
	   alsawork/alsawork.cpp \
	   alsawork/mixerswitches.cpp \
	   alsawork/volumemixers.cpp

HEADERS  += settingsdialog.h \
	    defines.h \
	    popupwindow.h \
	    alsawork/alsawork.h \
	    alsawork/mixerswitches.h \
	    alsawork/volumemixers.h

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
	translations.path = $$DATADIR/languages
	translations.extra = lrelease qtalvavolume.pro && cp -f $$LANG_PATH/qtalsavolume_*.qm  $(INSTALL_ROOT)$$translations.path
	icon1.path = $$DATADIR/icons
	icon1.files = \
		     icons/volume_ico.png
	licons.path = $$DATADIR/icons/light
	licons.files = icons/light/tb_icon0.png \
		       icons/light/tb_icon20.png \
		       icons/light/tb_icon40.png \
		       icons/light/tb_icon60.png \
		       icons/light/tb_icon80.png \
		       icons/light/tb_icon100.png
	dicons.path = $$DATADIR/icons/dark
	dicons.files = icons/dark/tb_icon0.png \
		       icons/dark/tb_icon20.png \
		       icons/dark/tb_icon40.png \
		       icons/dark/tb_icon60.png \
		       icons/dark/tb_icon80.png \
		       icons/dark/tb_icon100.png

	INSTALLS += dt \
		    translations \
		    icon1 \
		    licons \
		    dicons
}
