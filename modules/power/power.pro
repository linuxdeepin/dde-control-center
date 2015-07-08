TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         = power.h
SOURCES         = power.cpp
TARGET          = $$qtLibraryTarget(power)
DESTDIR         = $$_PRO_FILE_PWD_/../
