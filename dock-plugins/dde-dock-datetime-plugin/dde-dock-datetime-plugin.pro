#-------------------------------------------------
#
# Project created by QtCreator 2015-07-14T11:37:40
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = dde-dock-datetime-plugin
TEMPLATE = lib
CONFIG += plugin c++11

SOURCES += datetimeplugin.cpp \
    clockpixmap.cpp

HEADERS += datetimeplugin.h \
    clockpixmap.h

DISTFILES += dde-dock-datetime-plugin.json

target.path = /usr/share/dde-dock/plugins/

INSTALLS += target

RESOURCES += \
    images.qrc
