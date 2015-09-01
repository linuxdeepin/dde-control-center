#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T14:06:01
#
#-------------------------------------------------

QT       += core gui widgets dbus

TARGET = dde-dock-diskmount-plugin
TEMPLATE = lib
CONFIG += plugin c++11
DESTDIR = $$_PRO_FILE_PWD_/../

SOURCES += diskmountplugin.cpp \
    dbus/dbusdiskmount.cpp \
    mainitem.cpp \
    diskcontent.cpp \
    diskicon.cpp \
    diskitem.cpp \
    umountbutton.cpp

HEADERS += diskmountplugin.h \
    dbus/dbusdiskmount.h \
    mainitem.h \
    diskcontent.h \
    diskicon.h \
    diskitem.h \
    umountbutton.h
DISTFILES += dde-dock-diskmount-plugin.json

unix {
    target.path = /usr/share/dde-dock/plugins/
    INSTALLS += target
}

RESOURCES += \
    qss.qrc \
    disk-images.qrc

QMAKE_MOC_OPTIONS += -I/usr/include/
