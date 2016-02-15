
include(../../common.pri)

QT       += core gui widgets dbus svg

TARGET = dde-dock-bluetooth-plugin
TEMPLATE = lib
CONFIG += plugin c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget

DESTDIR = $$_PRO_FILE_PWD_/../

DISTFILES += dde-dock-bluetooth-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins
INSTALLS += target

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
