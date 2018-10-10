
#INCLUDEPATH += $$PWD/..

include(sections/sections.pri)
include(settings/settings.pri)

HEADERS += \
    $$PWD/networkmodule.h \
    $$PWD/networkmodulewidget.h \
    $$PWD/wirelesspage.h \
    $$PWD/accesspointwidget.h \
    $$PWD/vpnpage.h \
    $$PWD/connectioneditpagenew.h \
    $$PWD/connectionwirelesseditpage.h \
    $$PWD/connectionvpneditpage.h \
    $$PWD/connectionhotspoteditpage.h \
    $$PWD/pppoepage.h \
    $$PWD/networkdetailpage.h \
    $$PWD/proxypage.h \
    $$PWD/wiredpage.h \
    $$PWD/hotspotpage.h \
    $$PWD/chainsproxypage.h \
    $$PWD/chainstypepage.h

SOURCES += \
    $$PWD/networkmodule.cpp \
    $$PWD/networkmodulewidget.cpp \
    $$PWD/wirelesspage.cpp \
    $$PWD/accesspointwidget.cpp \
    $$PWD/vpnpage.cpp \
    $$PWD/connectioneditpagenew.cpp \
    $$PWD/connectionwirelesseditpage.cpp \
    $$PWD/connectionvpneditpage.cpp \
    $$PWD/connectionhotspoteditpage.cpp \
    $$PWD/pppoepage.cpp \
    $$PWD/networkdetailpage.cpp \
    $$PWD/proxypage.cpp \
    $$PWD/wiredpage.cpp \
    $$PWD/hotspotpage.cpp \
    $$PWD/chainsproxypage.cpp \
    $$PWD/chainstypepage.cpp

RESOURCES += \
    $$PWD/network.qrc

QT += network
PKGCONFIG += gio-2.0 libnm
LIBS += -L/usr/lib/x86_64-linux-gnu/ -lKF5NetworkManagerQt
INCLUDEPATH += /usr/include/KF5/NetworkManagerQt
DEPENDPATH += /usr/include/KF5/NetworkManagerQt
