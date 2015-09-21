include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

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

DISTFILES += sound.json

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target
