include(../../../common.pri)

QT       += core gui widgets dbus

TARGET = dde-dock-network-vpn-plugin
TEMPLATE = lib
CONFIG += c++11 link_pkgconfig plugin
DESTDIR = $$_PRO_FILE_PWD_/../

include(../network-data/network-data.pri)

INCLUDEPATH += ../../../modules/network

SOURCES += \
    ../../../modules/network/dbus/dbusnetwork.cpp \
    vpnplugin.cpp \
    vpnapplet.cpp \
    vpnappletitem.cpp

HEADERS += \
    ../../../modules/network/dbus/dbusnetwork.h \
    vpnplugin.h \
    vpnapplet.h \
    vpnappletitem.h

DISTFILES += dde-dock-network-vpn-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

QMAKE_MOC_OPTIONS += -I/usr/include/

RESOURCES += \
    resources.qrc
