include(../../common.pri)
TEMPLATE        = lib

CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
PKGCONFIG += dui
INCLUDEPATH    += ../../frame/ ../../widgets
LIBS += -L../../widgets -lwidgets
HEADERS         = power.h \
    dbus/dbuspower.h \
    trianglerectangle.h \
    powermanagement.h \
    presspowerbuttonaction.h \
    dextendboard.h \
    powerinterface.h

SOURCES         = power.cpp \
    dbus/dbuspower.cpp \
    trianglerectangle.cpp \
    powermanagement.cpp \
    presspowerbuttonaction.cpp \
    dextendboard.cpp \
    powerinterface.cpp

TARGET          = $$qtLibraryTarget(power)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES += power.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

isEqual(DCC_DISABLE_POWER_ACTION, YES){
    DEFINES += DCC_DISABLE_POWER_ACTION
}
