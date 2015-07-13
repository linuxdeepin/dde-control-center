#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T14:09:10
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = dde-dock-power-plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += powerplugin.cpp

HEADERS += powerplugin.h

DISTFILES += dde-dock-power-plugin.json

target.path = /usr/share/dde-dock/plugins/

INSTALLS += target
