#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T14:09:10
#
#-------------------------------------------------

QT       += core gui widgets dbus

TARGET = dde-dock-power-plugin
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../../modules/power

SOURCES += powerplugin.cpp \
    ../../modules/power/dbus/dbuspower.cpp

HEADERS += powerplugin.h \
    ../../modules/power/dbus/dbuspower.h

DISTFILES += dde-dock-power-plugin.json

target.path = /usr/share/dde-dock/plugins/

INSTALLS += target
