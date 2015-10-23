include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin  c++11 link_pkgconfig
QT             += widgets
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS         += network.h \
    networkmainwidget.h \
    dbus/dbusnetwork.h \
    wirednetworklistitem.h \
    wirelessnetworklistitem.h \
    vpnconnectswidget.h
SOURCES         += network.cpp \
    networkmainwidget.cpp \
    dbus/dbusnetwork.cpp \
    wirednetworklistitem.cpp \
    wirelessnetworklistitem.cpp \
    vpnconnectswidget.cpp
TARGET          = $$qtLibraryTarget(network)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += network.json

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target

RESOURCES += \
    theme.qrc
