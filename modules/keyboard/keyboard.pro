TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS        += keyboard.h \
                dbus/dbusinputdevices.h \
                dbus/dbuskeyboard.h
SOURCES        += keyboard.cpp\
                dbus/dbusinputdevices.cpp \
                dbus/dbuskeyboard.cpp
TARGET          = $$qtLibraryTarget(keyboard)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES      += keyboard.json

include(../../widgets/widgets.pri)
