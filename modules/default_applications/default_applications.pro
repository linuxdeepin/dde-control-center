include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS         = defaultapps.h \
    dbus/dbusdefaultmedia.h \
    dbus/dbusdefaultapps.h
SOURCES         = defaultapps.cpp \
    dbus/dbusdefaultmedia.cpp \
    dbus/dbusdefaultapps.cpp
TARGET          = $$qtLibraryTarget(defaultapps)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += default_applications.json

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target
