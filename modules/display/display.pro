TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS        += display.h \
    monitorground.h \
    monitor.h \
    singlemonitorsettings.h \
    dbus/displayinterface.h \
    dbus/monitorinterface.h
SOURCES        += display.cpp \
    monitorground.cpp \
    monitor.cpp \
    singlemonitorsettings.cpp \
    dbus/displayinterface.cpp \
    dbus/monitorinterface.cpp
TARGET          = $$qtLibraryTarget(display)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES += display.json

LIBS += -ldui
