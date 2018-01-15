
include(../../interfaces/interfaces.pri)

PLUGIN_NAME 	= wtf

QT              += widgets svg dbus
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dframeworkdbus

TARGET          = $$qtLibraryTarget($$PLUGIN_NAME)
DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += $$PLUGIN_NAME.json

HEADERS += \
    navgationplugin.h \
    navwidget.h \
    navitemwidget.h

SOURCES += \
    navgationplugin.cpp \
    navwidget.cpp \
    navitemwidget.cpp

target.path = $${PREFIX}/lib/dde-control-center/plugins/
INSTALLS += target

RESOURCES += \
    resources.qrc

isEqual(DISABLE_ACCOUNT, YES) {
    DEFINES += DISABLE_ACCOUNT
}

isEqual(DISABLE_DISPALY, YES) {
    DEFINES += DISABLE_DISPALY
}

isEqual(DISABLE_DEFAULT_APPLICATIONS, YES) {
    DEFINES += DISABLE_DEFAULT_APPLICATIONS
}

isEqual(DISABLE_PERSONALIZATION, YES) {
    DEFINES += DISABLE_PERSONALIZATION
}

isEqual(DISABLE_NETWORK, YES) {
    DEFINES += DISABLE_NETWORK
}

isEqual(DISABLE_SOUND, YES) {
    DEFINES += DISABLE_SOUND
}

isEqual(DISABLE_DATETIME, YES) {
    DEFINES += DISABLE_DATETIME
}

isEqual(DISABLE_POWER, YES) {
    DEFINES += DISABLE_POWER
}

isEqual(DISABLE_MOUSE, YES) {
    DEFINES += DISABLE_MOUSE
}

isEqual(DISABLE_KAYBOARD, YES) {
    DEFINES += DISABLE_KAYBOARD
}

isEqual(DISABLE_SYS_UPDATE, YES) {
    DEFINES += DISABLE_SYS_UPDATE
}

isEqual(DISABLE_BLUETOOTH, YES) {
    DEFINES += DISABLE_BLUETOOTH
}

isEqual(DISABLE_WACOM, YES) {
    DEFINES += DISABLE_WACOM
}
