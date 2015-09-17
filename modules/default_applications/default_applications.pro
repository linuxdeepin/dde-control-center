TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS         = defaultapps.h \
    dbus/dbusdefaultapps.h \
    dbus/dbusdefaultmedia.h \
    dbus/apptype.h
SOURCES         = defaultapps.cpp \
    dbus/dbusdefaultapps.cpp \
    dbus/dbusdefaultmedia.cpp \
    dbus/apptype.cpp
TARGET          = $$qtLibraryTarget(defaultapps)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += default_applications.json
