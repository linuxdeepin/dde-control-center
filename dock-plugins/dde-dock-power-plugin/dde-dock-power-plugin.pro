#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T14:09:10
#
#-------------------------------------------------
include(../../common.pri)

QT       += core gui widgets dbus

TARGET = dde-dock-power-plugin
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$_PRO_FILE_PWD_/../

INCLUDEPATH += ../../modules/power

SOURCES += powerplugin.cpp \
    ../../modules/power/dbus/dbuspower.cpp

HEADERS += powerplugin.h \
    ../../modules/power/dbus/dbuspower.h

DISTFILES += dde-dock-power-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

QMAKE_MOC_OPTIONS += -I/usr/include/
