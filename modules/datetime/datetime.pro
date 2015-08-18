TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS        += datetime.h
SOURCES        += datetime.cpp
TARGET          = $$qtLibraryTarget(datetime)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES      += datetime.json

include(../../widgets/widgets.pri)
