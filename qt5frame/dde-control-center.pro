TEMPLATE = app

QT += quick qml core widgets dbus

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

scripts.files = ../scripts/*
scripts.path = $$APPSHAREDIR/scripts

target.path = $$BINDIR
INSTALLS += target data scripts
