TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS         = keyboard.h 
SOURCES         = keyboard.cpp 
TARGET          = $$qtLibraryTarget(keyboard)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES      += keyboard.json

include(../../widgets/widgets.pri)
