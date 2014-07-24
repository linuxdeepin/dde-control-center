TEMPLATE = app

QT += quick qml core widgets dbus

SOURCES += main.cpp \
    qmlloader.cpp \
    python_module.cpp

HEADERS += \
    qmlloader.h \
    python_module.h

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

CONFIG += link_pkgconfig
PKGCONFIG += python2
