TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
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
CONFIG += c++11
