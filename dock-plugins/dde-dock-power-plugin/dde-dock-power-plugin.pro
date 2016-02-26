
include(../../common.pri)

QT       += core gui widgets dbus

TARGET = dde-dock-power-plugin
TEMPLATE = lib
CONFIG += plugin c++11
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
