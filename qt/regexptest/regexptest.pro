# -------------------------------------------------
# Project created by QtCreator 2010-07-02T12:02:50
# -------------------------------------------------
TEMPLATE = app
TARGET = regexptest

CONFIG += qt release

PREFIX = /usr
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share

SOURCES += main.cpp \
	   mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui
RESOURCES += resources.qrc

LANG_PATH = langs
TRANSLATIONS = $$LANG_PATH/regexptest_ru.ts \
	       $$LANG_PATH/regexptest_uk.ts

win32:RC_FILE = MyApp.rc
unix { 
	target.path = $$BINDIR
	INSTALLS += target
	df.path = $$DATADIR/applications/
	df.files = regexptest.desktop
	translations.path = $$DATADIR/regexptest/langs
	translations.extra = lrelease regexptest.pro && cp -f $$LANG_PATH/regexptest_*.qm  $(INSTALL_ROOT)$$translations.path
	icon.path =$$DATADIR/regexptest/icons
	icon.files = icons/72x72.png
	docs.path = $$DATADIR/doc/regexptest/html
	docs.files = docs/regexp_help.html
	INSTALLS += df \
		    translations \
		    icon \
		    docs
}
