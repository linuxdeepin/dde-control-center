TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         = sound.h
SOURCES         = sound.cpp
TARGET          = $$qtLibraryTarget(sound)
DESTDIR         = $$_PRO_FILE_PWD_/../

EXAMPLE_FILES = sound.json
