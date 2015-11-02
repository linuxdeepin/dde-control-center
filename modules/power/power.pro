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
    powerinterfacemanagement.h

SOURCES         = power.cpp \
    dbus/dbuspower.cpp \
    trianglerectangle.cpp \
    powermanagement.cpp \
    presspowerbuttonaction.cpp \
    dextendboard.cpp \
    powerinterfacemanagement.cpp

TARGET          = $$qtLibraryTarget(power)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES += power.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
