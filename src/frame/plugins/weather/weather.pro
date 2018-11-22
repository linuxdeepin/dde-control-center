
include(../../interfaces/interfaces.pri)
include(../../widgets/widgets.pri)
include(./caiyun/caiyun.pri)
include(./deepin/deepin.pri)

PLUGIN_NAME 	= weather

QT              += widgets svg network xml
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dtkwidget geoip

TARGET          = $$qtLibraryTarget($$PLUGIN_NAME)
DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += $$PLUGIN_NAME.json

# fix compiler error at gcc6
QMAKE_CFLAGS_ISYSTEM = ""

HEADERS += \
    weatheritem.h \
    weatherrequest.h \
    weatherwidget.h \
    weatherplugin.h \
    networkutil.h \
    types.h \
    setlocationpage.h \
    locationprovider.h

SOURCES += \
    weatheritem.cpp \
    weatherrequest.cpp \
    weatherwidget.cpp \
    weatherplugin.cpp \
    networkutil.cpp \
    setlocationpage.cpp

RESOURCES += \
    weather.qrc

target.path = $${PREFIX}/lib/dde-control-center/plugins/
INSTALLS += target
