TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS         = mouse.h \
    dbus/dbusmouse.h \
    dbus/dbustouchpad.h
SOURCES         = mouse.cpp \
    dbus/dbusmouse.cpp \
    dbus/dbustouchpad.cpp
TARGET          = $$qtLibraryTarget(mouse)
DESTDIR         = $$_PRO_FILE_PWD_/../

include(../../widgets/widgets.pri)
