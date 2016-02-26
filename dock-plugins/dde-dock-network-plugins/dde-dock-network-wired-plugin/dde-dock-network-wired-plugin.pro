
include(../../../common.pri)

QT       += core gui widgets dbus

TARGET = dde-dock-network-wired-plugin
TEMPLATE = lib
CONFIG += c++11 link_pkgconfig plugin
DESTDIR = $$_PRO_FILE_PWD_/../

include(../network-data/network-data.pri)

INCLUDEPATH += ../../../modules/network

SOURCES += wiredplugin.cpp \
    ../../../modules/network/dbus/dbusnetwork.cpp

HEADERS += wiredplugin.h \
    ../../../modules/network/dbus/dbusnetwork.h

DISTFILES += dde-dock-network-wired-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

QMAKE_MOC_OPTIONS += -I/usr/include/

RESOURCES += \
    resources.qrc
