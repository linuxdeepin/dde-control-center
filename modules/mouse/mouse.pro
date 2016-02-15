
include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dtkbase dtkwidget
LIBS += -L../../widgets -lwidgets

HEADERS         = mouse.h \
    dbus/dbusmouse.h \
    dbus/dbustouchpad.h
SOURCES         = mouse.cpp \
    dbus/dbusmouse.cpp \
    dbus/dbustouchpad.cpp
TARGET          = $$qtLibraryTarget(mouse)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += mouse.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
