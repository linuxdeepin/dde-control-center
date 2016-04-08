
include(../../common.pri)

TEMPLATE        = lib

CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
PKGCONFIG += dtkbase dtkwidget
INCLUDEPATH    += ../../frame/ ../../widgets
LIBS += -L../../widgets -lwidgets
HEADERS         = power.h \
    powermanagement.h \
    presspowerbuttonaction.h \
    widgets/doublecornerwidget.h \
    dbus/dbuspower.h \
    powerinterface.h \
    widgets/powerswitchwidget.h \
    widgets/choosedelaytimewidget.h

SOURCES         = power.cpp \
    powermanagement.cpp \
    presspowerbuttonaction.cpp \
    widgets/doublecornerwidget.cpp \
    dbus/dbuspower.cpp \
    powerinterface.cpp \
    widgets/powerswitchwidget.cpp \
    widgets/choosedelaytimewidget.cpp

TARGET          = $$qtLibraryTarget(power)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES += power.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

isEqual(DCC_DISABLE_POWER_ACTION, YES){
    DEFINES += DCC_DISABLE_POWER_ACTION
}
