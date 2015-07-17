TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         = bluetooth.h
SOURCES         = bluetooth.cpp
TARGET          = $$qtLibraryTarget(bluetooth)
DESTDIR         = $$_PRO_FILE_PWD_/../
