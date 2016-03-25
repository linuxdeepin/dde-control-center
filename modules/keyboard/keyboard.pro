
include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS        += keyboard.h \
                dbus/dbusinputdevices.h \
                dbus/dbuskeyboard.h \
    firstletterclassify.h \
    dbus/dbuslangselector.h
SOURCES        += keyboard.cpp\
                dbus/dbusinputdevices.cpp \
                dbus/dbuskeyboard.cpp \
    firstletterclassify.cpp \
    dbus/dbuslangselector.cpp
TARGET          = $$qtLibraryTarget(keyboard)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES      += keyboard.json

RESOURCES += \
    theme.qrc \
    images.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

isEqual(DCC_DISABLE_LANG_SETTING, YES){
    DEFINES += DCC_DISABLE_LANG_SETTING
}
