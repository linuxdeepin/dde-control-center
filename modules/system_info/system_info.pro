include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
LIBS += -L../../widgets -lwidgets

HEADERS         = systeminfo.h \
    dbus/dbussysteminfo.h \
    updatearrowexpand.h \
    mirrorscontrolwidget.h \
    mirroritemwidget.h \
    updatewidget.h \
    separatorwidget.h \
    applictionitemwidget.h \
    dbus/dbuslastoremanager.h \
    dbus/mirrorinfo.h \
    dbus/appupdateinfo.h \
    dbus/dbusupdatejobmanager.h \
    dbus/dbusupdatejob.h
SOURCES         = systeminfo.cpp \
    dbus/dbussysteminfo.cpp \
    updatearrowexpand.cpp \
    mirrorscontrolwidget.cpp \
    mirroritemwidget.cpp \
    updatewidget.cpp \
    separatorwidget.cpp \
    applictionitemwidget.cpp \
    dbus/dbuslastoremanager.cpp \
    dbus/mirrorinfo.cpp \
    dbus/appupdateinfo.cpp \
    dbus/dbusupdatejobmanager.cpp \
    dbus/dbusupdatejob.cpp
TARGET          = $$qtLibraryTarget(systeminfo)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += system_info.json

RESOURCES += \
    resource.qrc \
    theme.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
