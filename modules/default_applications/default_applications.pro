TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
LIBS           += -ldui
INCLUDEPATH    += ../../frame/
HEADERS         = defaultapps.h \
    dbus/dbusdefaultapps.h
SOURCES         = defaultapps.cpp \
    dbus/dbusdefaultapps.cpp
TARGET          = $$qtLibraryTarget(defaultapps)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += default_applications.json
