TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         = network.h
SOURCES         = network.cpp
TARGET          = $$qtLibraryTarget(network)
DESTDIR         = $$_PRO_FILE_PWD_/../
