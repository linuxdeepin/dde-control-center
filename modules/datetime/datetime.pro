
include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dtkbase dtkwidget
LIBS += -L../../widgets -lwidgets

HEADERS        += datetime.h \
    dbus/dbustimedate.h \
    timewidget.h \
    datecontrolwidget.h \
    timezonectrlwidget.h \
    timezonewidget.h \
    timezoneitemwidget.h
SOURCES        += datetime.cpp \
    dbus/dbustimedate.cpp \
    timewidget.cpp \
    datecontrolwidget.cpp \
    timezonectrlwidget.cpp \
    timezonewidget.cpp \
    timezoneitemwidget.cpp
TARGET          = $$qtLibraryTarget(datetime)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES      += datetime.json

RESOURCES += \
    theme.qrc \
    resource.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
