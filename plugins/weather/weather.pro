
include(../../interfaces/interfaces.pri)

PLUGIN_NAME 	= weather

QT              += widgets svg network
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       +=

TARGET          = $$qtLibraryTarget($$PLUGIN_NAME)
DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += $$PLUGIN_NAME.json

HEADERS += \
    weatheritem.h \
    weatherinterface.h \
    weatherrequest.h \
    weatherwidget.h \
    weatherplugin.h

SOURCES += \
    weatheritem.cpp \
    weatherinterface.cpp \
    weatherrequest.cpp \
    weatherwidget.cpp \
    weatherplugin.cpp

RESOURCES += \
    weather.qrc

target.path = $${PREFIX}/lib/dde-control-center/plugins/
INSTALLS += target
