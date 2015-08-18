#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T15:26:32
#
#-------------------------------------------------

QT       += core gui widgets

TEMPLATE = lib
CONFIG += plugin c++11
LIBS += -ldui
INCLUDEPATH += ../../frame/
INCLUDEPATH += /usr/include/libdui/
DESTDIR         = $$_PRO_FILE_PWD_/../

SOURCES += personalization.cpp
HEADERS += personalization.h
DISTFILES += \
    personalization.json

TARGET          = $$qtLibraryTarget(personalization)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
