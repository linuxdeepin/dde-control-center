
include(../../interfaces/interfaces.pri)

PLUGIN_NAME 	= calculator

QT              += widgets svg
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       +=

TARGET          = $$qtLibraryTarget($$PLUGIN_NAME)
DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += $$PLUGIN_NAME.json

HEADERS += \
    calculatorplugin.h

SOURCES += \
    calculatorplugin.cpp

target.path = $${PREFIX}/lib/dde-control-center/plugins/
INSTALLS += target
