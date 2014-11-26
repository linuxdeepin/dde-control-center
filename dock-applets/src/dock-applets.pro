TEMPLATE = app
TARGET = dde-dock-applets

QT += quick qml core dbus widgets

SOURCES += \
    main.cpp \
    qmlloader.cpp

HEADERS += \
    qmlloader.h

RESOURCES += \
    frame.qrc \

#VARIABLES
isEmpty(PREFIX) {
    PREFIX = /usr
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
DOCKAPPLETSDIR = $$DATADIR/dde-control-center/dock-applets

DEFINES += DATADIR=\\\"$$DATADIR\\\" DOCKAPPLETSDIR=\\\"$$DOCKAPPLETSDIR\\\"

#MAKE INSTALL

target.path =$$BINDIR

icons.files = ../data/icons/*
icons.path = $$DOCKAPPLETSDIR/icons

default_icons_16.files = ../data/16/*
default_icons_16.path = /usr/share/icons/hicolor/16x16/status

default_icons_48.files = ../data/48/*
default_icons_48.path = /usr/share/icons/hicolor/48x48/status

plugins.files = ..//plugins/*
plugins.path = $$DOCKAPPLETSDIR/plugins

INSTALLS += target icons plugins default_icons_16 default_icons_48

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0
