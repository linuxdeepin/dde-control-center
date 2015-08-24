TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS         = mouse.h \
    dbus/dbusmouse.h \
    dbus/dbustouchpad.h
SOURCES         = mouse.cpp \
    dbus/dbusmouse.cpp \
    dbus/dbustouchpad.cpp
TARGET          = $$qtLibraryTarget(mouse)
DESTDIR         = $$_PRO_FILE_PWD_/../
