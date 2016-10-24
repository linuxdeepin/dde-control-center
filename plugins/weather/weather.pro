
include(../../interfaces/interfaces.pri)

PLUGIN_NAME 	= weather

QT              += widgets svg network
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += geoip

TARGET          = $$qtLibraryTarget($$PLUGIN_NAME)
DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += $$PLUGIN_NAME.json

# fix complier error at gcc6
QMAKE_CFLAGS_ISYSTEM = ""

HEADERS += \
    weatheritem.h \
    weatherinterface.h \
    weatherrequest.h \
    weatherwidget.h \
    weatherplugin.h \
    networkutil.h

SOURCES += \
    weatheritem.cpp \
    weatherinterface.cpp \
    weatherrequest.cpp \
    weatherwidget.cpp \
    weatherplugin.cpp \
    networkutil.cpp

RESOURCES += \
    weather.qrc

target.path = $${PREFIX}/lib/dde-control-center/plugins/
INSTALLS += target
