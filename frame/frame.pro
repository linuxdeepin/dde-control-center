
include(../interfaces/interfaces.pri)
include(../widgets/widgets.pri)

# modules
include(../modules/modules.pri)

QT += widgets dbus svg sql x11extras concurrent

TARGET     = dde-control-center
DESTDIR    = $$_PRO_FILE_PWD_/../
TEMPLATE   = app
CONFIG += c++11 link_pkgconfig
LIBS += -L../widgets
PKGCONFIG      += dtkwidget dtkbase dtkutil dframeworkdbus xcb

HEADERS    += \
    frame.h \
    pluginscontroller.h \
    framewidget.h \
    mainwidget.h \
    settingswidget.h \
    moduleinitthread.h \
    dbuscontrolcenterservice.h \
    quick_control/quickcontrolpanel.h \
    quick_control/basicsettingspage.h \
    quick_control/quickswitchbutton.h \
    quick_control/vpn/vpncontrolpage.h \
    quick_control/vpn/vpnlistmodel.h \
    quick_control/basiclistview.h \
    quick_control/display/displaycontrolpage.h \
    quick_control/display/displaycontrolmodel.h \
    quick_control/wifi/wifipage.h \
    quick_control/wifi/wifilistmodel.h

SOURCES    += \
            main.cpp \
    frame.cpp \
    pluginscontroller.cpp \
    framewidget.cpp \
    mainwidget.cpp \
    settingswidget.cpp \
    moduleinitthread.cpp \
    dbuscontrolcenterservice.cpp \
    quick_control/quickcontrolpanel.cpp \
    quick_control/basicsettingspage.cpp \
    quick_control/quickswitchbutton.cpp \
    quick_control/vpn/vpncontrolpage.cpp \
    quick_control/vpn/vpnlistmodel.cpp \
    quick_control/basiclistview.cpp \
    quick_control/display/displaycontrolpage.cpp \
    quick_control/display/displaycontrolmodel.cpp \
    quick_control/wifi/wifipage.cpp \
    quick_control/wifi/wifilistmodel.cpp

#RESOURCES += \
#    qss.qrc \
#    images.qrc

#INCLUDEPATH    += ../widgets ../modules/display

#include(../interfaces/interfaces.pri)

#isEqual(DCC_DISABLE_ANIMATION, YES){
#    DEFINES += DCC_DISABLE_ANIMATION
#}

#isEqual(DCC_CACHE_MODULES, YES){
#    DEFINES += DCC_CACHE_MODULES
#}

RESOURCES += \
    frame.qrc

isEqual(DISABLE_SYS_UPDATE, YES){
    DEFINES += DISABLE_SYS_UPDATE
}
