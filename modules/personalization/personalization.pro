
include(../../common.pri)

QT       += core gui widgets dbus
TEMPLATE = lib
CONFIG += plugin c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget
LIBS += -L../../widgets -lwidgets
INCLUDEPATH += ../../frame/ ../../widgets
INCLUDEPATH += /usr/include/
DESTDIR         = $$_PRO_FILE_PWD_/../

SOURCES += personalization.cpp \
    dbus/appearancedaemon_interface.cpp \
    dbusworker.cpp
HEADERS += personalization.h \
    dbus/appearancedaemon_interface.h \
    dbusworker.h \
    persionalizationfwd.h
DISTFILES += \
    personalization.json \
    dbus/appearancedaemon.xml

TARGET          = $$qtLibraryTarget(personalization)

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

RESOURCES +=
# fix complier error at gcc6
QMAKE_CFLAGS_ISYSTEM = ""
