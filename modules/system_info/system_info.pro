include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
LIBS += -L../../widgets -lwidgets

HEADERS         = systeminfo.h \
    dbus/dbussysteminfo.h
SOURCES         = systeminfo.cpp \
    dbus/dbussysteminfo.cpp
TARGET          = $$qtLibraryTarget(systeminfo)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += system_info.json

RESOURCES += \
    resource.qrc

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target
