
include(../../common.pri)

QT       += core gui widgets

TARGET = dde-dock-datetime-plugin
TEMPLATE = lib
CONFIG += plugin c++11
INCLUDEPATH += ../../modules/datetime/
DESTDIR = $$_PRO_FILE_PWD_/../

SOURCES += datetimeplugin.cpp \
    clockpixmap.cpp \
    ../../modules/datetime/dbus/dbustimedate.cpp

HEADERS += datetimeplugin.h \
    clockpixmap.h \
    ../../modules/datetime/dbus/dbustimedate.h

DISTFILES += dde-dock-datetime-plugin.json

target.path = $${PREFIX}/lib/dde-dock/plugins
INSTALLS += target

RESOURCES += \
    images.qrc

LIBS += -ldui

QMAKE_MOC_OPTIONS += -I/usr/include/
