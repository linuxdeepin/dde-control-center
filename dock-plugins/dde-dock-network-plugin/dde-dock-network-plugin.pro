#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T14:09:10
#
#-------------------------------------------------

QT       += core gui widgets dbus

TARGET = dde-dock-network-plugin
TEMPLATE = lib
CONFIG += plugin c++11

INCLUDEPATH += ../../modules/bluetooth

SOURCES += plugins.cpp \ 
    bluetoothcomponent.cpp \
    widgets/switchbutton.cpp \
    ../../modules/bluetooth/dbus/dbusbluetooth.cpp

HEADERS += plugins.h \ 
    plugincomponentinterface.h \
    bluetoothcomponent.h \
    widgets/switchbutton.h \
    ../../modules/bluetooth/dbus/dbusbluetooth.h

DISTFILES += dde-dock-network-plugin.json

target.path = /usr/share/dde-dock/plugins/

INSTALLS += target

RESOURCES += \
    images.qrc \
    qss.qrc
