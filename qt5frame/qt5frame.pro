TEMPLATE = app
TARGET = dde-control-center

QT += quick qml core widgets dbus gui

SOURCES += main.cpp \
    qmlloader.cpp

HEADERS += \
    qmlloader.h

RESOURCES += \
    views.qrc \
    modules.qrc

isEmpty(PREFIX){
    PREFIX = /usr
}

BINDIR = $$PREFIX/bin
APPSHAREDIR = $$PREFIX/share/dde-control-center
DEFINES += APPSHAREDIR=\\\"$$APPSHAREDIR\\\"

data.files = ../data/*
data.path = $$APPSHAREDIR/data

target.path = $$BINDIR
INSTALLS += target data

OTHER_FILES +=
