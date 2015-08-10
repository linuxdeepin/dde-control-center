TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS         = systeminfo.h \
    dbus/dbussysteminfo.h
SOURCES         = systeminfo.cpp \
    dbus/dbussysteminfo.cpp
TARGET          = $$qtLibraryTarget(systeminfo)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES +=

RESOURCES +=

QMAKE_CXXFLAGS += -std=c++11
