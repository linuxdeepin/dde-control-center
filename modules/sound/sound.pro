
include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dtkbase dtkwidget
LIBS += -L../../widgets -lwidgets

HEADERS         = sound.h \
    dbus/dbusaudio.h \
    dbus/dbusaudiosink.h \
    headerline.h \
    dbus/dbusaudiosinkinput.h \
    dbus/dbusaudiosource.h \
    soundeffectswitchwidget.h \
    dbus/dbussoundeffects.h \
    soundview.h \
    soundcontrol.h
SOURCES         = sound.cpp \
    dbus/dbusaudio.cpp \
    dbus/dbusaudiosink.cpp \
    headerline.cpp \
    dbus/dbusaudiosinkinput.cpp \
    dbus/dbusaudiosource.cpp \
    soundeffectswitchwidget.cpp \
    dbus/dbussoundeffects.cpp \
    soundview.cpp \
    soundcontrol.cpp
TARGET          = $$qtLibraryTarget(sound)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += sound.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

isEqual(DCC_DISABLE_MICROPHONE_FEEDBACK, YES){
    DEFINES += DCC_DISABLE_MICROPHONE_FEEDBACK
}
