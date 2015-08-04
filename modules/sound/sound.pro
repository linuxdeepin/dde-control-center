TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS         = sound.h \
    dbus/dbusaudio.h \
    dbus/dbusaudiosink.h
SOURCES         = sound.cpp \
    dbus/dbusaudio.cpp \
    dbus/dbusaudiosink.cpp
TARGET          = $$qtLibraryTarget(sound)
DESTDIR         = $$_PRO_FILE_PWD_/../
