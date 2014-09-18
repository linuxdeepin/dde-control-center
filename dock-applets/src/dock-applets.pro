TEMPLATE = app
TARGET = dde-dock-applets

QT += quick qml core dbus

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

plugins.files = ..//plugins/*
plugins.path = $$DOCKAPPLETSDIR/plugins

INSTALLS += target icons plugins
