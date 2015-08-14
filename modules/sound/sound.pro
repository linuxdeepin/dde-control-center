TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS         = sound.h \
    dbus/dbusaudio.h \
    dbus/dbusaudiosink.h \
    headerline.h \
    dbus/dbusaudiosinkinput.h \
    dbus/dbusaudiosource.h
SOURCES         = sound.cpp \
    dbus/dbusaudio.cpp \
    dbus/dbusaudiosink.cpp \
    headerline.cpp \
    dbus/dbusaudiosinkinput.cpp \
    dbus/dbusaudiosource.cpp
TARGET          = $$qtLibraryTarget(sound)
DESTDIR         = $$_PRO_FILE_PWD_/../

include(../../widgets/widgets.pri)
