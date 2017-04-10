
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
