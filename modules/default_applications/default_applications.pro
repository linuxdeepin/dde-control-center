TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         = defaultapps.h
SOURCES         = defaultapps.cpp
TARGET          = $$qtLibraryTarget(defaultapps)
DESTDIR         = $$_PRO_FILE_PWD_/../
