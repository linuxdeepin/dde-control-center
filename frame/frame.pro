
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
PKGCONFIG      += dtkwidget dtkbase dtkutil dframeworkdbus xcb xext gsettings-qt

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
    quick_control/display/displaycontrolpage.h \
    quick_control/display/displaycontrolmodel.h \
    quick_control/wifi/wifipage.h \
    quick_control/wifi/wifilistmodel.h \
    quick_control/bluetooth/bluetoothlist.h \
    quick_control/bluetooth/bluetoothlistmodel.h \
    quick_control/vpn/vpnlistdelegate.h \
    quick_control/wifi/wifilistdelegate.h \
    quick_control/display/displayitemdelegate.h \
    quick_control/bluetooth/bluetoothdelegate.h \
    navgationview.h \
    navgationmodel.h \
    navgationdelegate.h \
    indicatorwidget.h \
    updatenotifier.h

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
    quick_control/display/displaycontrolpage.cpp \
    quick_control/display/displaycontrolmodel.cpp \
    quick_control/wifi/wifipage.cpp \
    quick_control/wifi/wifilistmodel.cpp \
    quick_control/bluetooth/bluetoothlist.cpp \
    quick_control/bluetooth/bluetoothlistmodel.cpp \
    quick_control/vpn/vpnlistdelegate.cpp \
    quick_control/wifi/wifilistdelegate.cpp \
    quick_control/display/displayitemdelegate.cpp \
    quick_control/bluetooth/bluetoothdelegate.cpp \
    navgationview.cpp \
    navgationmodel.cpp \
    navgationdelegate.cpp \
    indicatorwidget.cpp \
    updatenotifier.cpp

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

isEqual(DISABLE_SYS_UPDATE, YES) {
    DEFINES += DISABLE_SYS_UPDATE
}

isEqual(DISABLE_SYS_UPDATE_MIRRORS, YES) {
    DEFINES += DISABLE_SYS_UPDATE_MIRRORS
}

isEqual(DCC_DISABLE_OPACITY_ANIMATION, YES) {
    DEFINES += DISABLE_OPACITY_ANIMATION
}

isEqual(DCC_DISABLE_MIRACAST, YES) {
    DEFINES += DCC_DISABLE_MIRACAST
}

isEqual(DCC_DISABLE_TIMEZONE, YES) {
    DEFINES += DCC_DISABLE_TIMEZONE
}

isEqual(DCC_DISABLE_KBLAYOUT, YES) {
    DEFINES += DCC_DISABLE_KBLAYOUT
}

isEqual(DCC_DISABLE_LANGUAGE, YES) {
    DEFINES += DCC_DISABLE_LANGUAGE
}

isEqual(DCC_DISABLE_GRUB, YES) {
    DEFINES += DCC_DISABLE_GRUB
}

isEqual(DCC_KEEP_SETTINGS_LIVE, YES) {
    DEFINES += DCC_KEEP_SETTINGS_LIVE
}

isEqual(DCC_DISABLE_FEEDBACK, YES) {
    DEFINES += DCC_DISABLE_FEEDBACK
}

isEqual(DCC_DISABLE_ROTATE, YES) {
    DEFINES += DCC_DISABLE_ROTATE
}
