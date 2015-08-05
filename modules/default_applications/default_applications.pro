TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
LIBS           += -ldui
INCLUDEPATH    += ../../frame/
HEADERS         = defaultapps.h
SOURCES         = defaultapps.cpp
TARGET          = $$qtLibraryTarget(defaultapps)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += default_applications.json
