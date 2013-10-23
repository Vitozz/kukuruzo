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
