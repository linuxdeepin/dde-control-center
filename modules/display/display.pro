include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS        += display.h \
    monitorground.h \
    monitor.h \
    dbus/displayinterface.h \
    dbus/monitorinterface.h \
    displaymodeitem.h \
    titleandwidget.h \
    customsettings.h \
    fullscreentooltip.h
SOURCES        += display.cpp \
    monitorground.cpp \
    monitor.cpp \
    dbus/displayinterface.cpp \
    dbus/monitorinterface.cpp \
    displaymodeitem.cpp \
    titleandwidget.cpp \
    customsettings.cpp \
    fullscreentooltip.cpp
TARGET          = $$qtLibraryTarget(display)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES += display.json

LIBS += -ldui

RESOURCES += \
    theme.qrc

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target
