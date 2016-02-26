
include(../../../common.pri)

QT       += core gui widgets dbus

TARGET = dde-dock-network-wireless-plugin
TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$_PRO_FILE_PWD_/../

include(../network-data/network-data.pri)

INCLUDEPATH += ../../../modules/network

CONFIG += c++11 link_pkgconfig

SOURCES += wirelessplugin.cpp \
    ../../../modules/network/dbus/dbusnetwork.cpp \
    wirelessappletitem.cpp \
    wirelessapplet.cpp \
    wirelessitem.cpp

HEADERS += wirelessplugin.h \
    ../../../modules/network/dbus/dbusnetwork.h \
    wirelessappletitem.h \
    wirelessapplet.h \
    wirelessitem.h \
    ../../../modules/network/networkglobal.h

DISTFILES += dde-dock-network-wireless-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

QMAKE_MOC_OPTIONS += -I/usr/include/

RESOURCES += \
    resources.qrc
