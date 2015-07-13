#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T10:27:10
#
#-------------------------------------------------

QT       += core gui

TARGET = dde-dock-sound-plugin
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/generic

SOURCES += soundplugin.cpp

HEADERS += soundplugin.h
DISTFILES += dde-dock-sound-plugin.json

unix {
    target.path = /usr/share/dde-dock/plugins/
    INSTALLS += target
}
