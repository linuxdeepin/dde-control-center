#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T15:26:32
#
#-------------------------------------------------

QT       += core gui widgets dbus

include(../../common.pri)
TEMPLATE = lib
CONFIG += plugin c++11
LIBS += -ldui -L../../widgets -lwidgets
INCLUDEPATH += ../../frame/ ../../widgets
INCLUDEPATH += /usr/include/libdui/
DESTDIR         = $$_PRO_FILE_PWD_/../

SOURCES += personalization.cpp \
    dbus/appearancedaemon_interface.cpp \
    dbusworker.cpp \
    previewwindow.cpp
HEADERS += personalization.h \
    dbus/appearancedaemon_interface.h \
    dbusworker.h \
    persionalizationfwd.h \
    previewwindow.h
DISTFILES += \
    personalization.json \
    dbus/appearancedaemon.xml

TARGET          = $$qtLibraryTarget(personalization)

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target

RESOURCES += images.qrc
