
include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dtkbase dtkwidget
LIBS += -L../../widgets -lwidgets

QT             += widgets dbus
HEADERS         += bluetooth.h \
    bluetoothmainwidget.h \
    dbus/dbusbluetooth.h \
    adapterwidget.h \
    deviceitemwidget.h \
    confrimwidget.h
SOURCES         += bluetooth.cpp \
    bluetoothmainwidget.cpp \
    dbus/dbusbluetooth.cpp \
    adapterwidget.cpp \
    deviceitemwidget.cpp \
    confrimwidget.cpp
TARGET          = $$qtLibraryTarget(bluetooth)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += bluetooth.json

RESOURCES += \
    theme.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
