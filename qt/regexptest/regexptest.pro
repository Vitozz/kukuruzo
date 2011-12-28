# -------------------------------------------------
# Project created by QtCreator 2010-07-02T12:02:50
# -------------------------------------------------
include(conf.pri)
TEMPLATE = app
TARGET = regexptest
CONFIG += qt release
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui
RESOURCES += resources.qrc
win32:RC_FILE = MyApp.rc
unix { 
    target.path = $$BINDIR
    INSTALLS += target
    dt.path = $$PREFIX/share/applications/
    dt.files = regexptest.desktop
    icon1.path = $$PREFIX/share/regexptest/icons
    icon1.files = icons/24x24.png
    icon2.path = $$PREFIX/share/regexptest/icons
    icon2.files = icons/36x36.png
    icon3.path = $$PREFIX/share/regexptest/icons
    icon3.files = icons/48x48.png
    icon4.path =$$PREFIX/share/regexptest/icons
    icon4.files = icons/72x72.png
    docs.path = $$PREFIX/share/doc/regexptest/html
    docs.files = docs/regexp_help.html 
    INSTALLS += dt \
        icon1 \
        icon2 \
        icon3 \
        icon4 \
        docs
}