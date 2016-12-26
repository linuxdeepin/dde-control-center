
include(../../interfaces/interfaces.pri)

PLUGIN_NAME 	= example

QT              += widgets svg
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       +=

TARGET          = $$qtLibraryTarget($$PLUGIN_NAME)
DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += $$PLUGIN_NAME.json

HEADERS += \
    exampleplugin.h

SOURCES += \
    exampleplugin.cpp

target.path = $${PREFIX}/lib/dde-control-center/plugins/
INSTALLS += target
