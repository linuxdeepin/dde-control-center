
include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets ../../helper
LIBS += -L../../widgets -lwidgets -L../../helper -lhelper

HEADERS         = systeminfo.h \
    dbus/dbussysteminfo.h \
    updatearrowexpand.h \
    mirrorscontrolwidget.h \
    mirroritemwidget.h \
    updatewidget.h \
    separatorwidget.h \
    applictionitemwidget.h \
    dbus/mirrorinfo.h \
    dbus/appupdateinfo.h \
    dbus/dbusupdatejobmanager.h \
    dbus/dbusupdatejob.h \
    dbus/dbuslastoreupdater.h \
    licensescanner.h
SOURCES         = systeminfo.cpp \
    dbus/dbussysteminfo.cpp \
    updatearrowexpand.cpp \
    mirrorscontrolwidget.cpp \
    mirroritemwidget.cpp \
    updatewidget.cpp \
    separatorwidget.cpp \
    applictionitemwidget.cpp \
    dbus/mirrorinfo.cpp \
    dbus/appupdateinfo.cpp \
    dbus/dbusupdatejobmanager.cpp \
    dbus/dbusupdatejob.cpp \
    dbus/dbuslastoreupdater.cpp \
    licensescanner.cpp
TARGET          = $$qtLibraryTarget(systeminfo)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += system_info.json

RESOURCES += \
    resource.qrc \
    theme.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

isEmpty(WITH_MODULE_SYSINFO_UPDATE) {
    WITH_MODULE_SYSINFO_UPDATE = YES
}

isEqual(WITH_MODULE_SYSINFO_UPDATE, YES) {
    DEFINES += DCC_SYSINFO_UPDATE
}
