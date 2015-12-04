include(../../../common.pri)

QT       += core gui widgets dbus

TARGET = dde-dock-network-complex-plugin
TEMPLATE = lib
CONFIG += c++11 link_pkgconfig plugin
DESTDIR = $$_PRO_FILE_PWD_/../

include(../network-data/network-data.pri)

SOURCES += \
    complexapplet.cpp \
    complexappletitem.cpp \
    complexplugin.cpp \
    complexitem.cpp \
    ../../../modules/bluetooth/dbus/dbusbluetooth.cpp \
    ../../../modules/network/dbus/dbusnetwork.cpp

HEADERS += \
    complexapplet.h \
    complexappletitem.h \
    complexplugin.h \
    complexitem.h \
    ../../../modules/bluetooth/dbus/dbusbluetooth.h \
    ../../../modules/network/dbus/dbusnetwork.h

DISTFILES += dde-dock-network-complex-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

QMAKE_MOC_OPTIONS += -I/usr/include/

RESOURCES += \
    resources.qrc
