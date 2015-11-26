#-------------------------------------------------
#
# Project created by QtCreator 2015-07-14T11:37:40
#
#-------------------------------------------------
include(../../common.pri)

QT       += core gui widgets dbus svg

TARGET = dde-dock-bluetooth-plugin
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$_PRO_FILE_PWD_/../

DISTFILES += dde-dock-bluetooth-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins
INSTALLS += target

LIBS += -ldui

QMAKE_MOC_OPTIONS += -I/usr/include/

HEADERS += \
    bluetoothplugin.h \
    dbus/dbusbluetooth.h \
    adapterwidget.h \
    deviceitemwidget.h \
    constants.h \
    bluetoothobject.h

SOURCES += \
    bluetoothplugin.cpp \
    dbus/dbusbluetooth.cpp \
    adapterwidget.cpp \
    deviceitemwidget.cpp \
    bluetoothobject.cpp

RESOURCES += $$PWD/theme.qrc
