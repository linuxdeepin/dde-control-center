TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS        += display.h \
    monitorground.h \
    monitor.h \
    singlemonitorsettings.h
SOURCES        += display.cpp \
    monitorground.cpp \
    monitor.cpp \
    singlemonitorsettings.cpp
TARGET          = $$qtLibraryTarget(display)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES += display.json

LIBS += -ldui
