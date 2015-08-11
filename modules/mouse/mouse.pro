TEMPLATE        = lib
CONFIG         += plugin
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
