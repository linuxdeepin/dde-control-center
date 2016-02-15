
include(../../interfaces/interfaces.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dtkbase dtkwidget
LIBS += -L../../widgets -lwidgets

HEADERS         = \
    dbus/dbusinputwacom.h \
    wacom.h
SOURCES         = \
    wacom.cpp \
    dbus/dbusinputwacom.cpp
TARGET          = $$qtLibraryTarget(wacom)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += \
    wacom.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
