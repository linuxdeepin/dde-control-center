
include(../interfaces/interfaces.pri)

QT += widgets dbus svg

TARGET     = dde-control-center
DESTDIR    = $$_PRO_FILE_PWD_/../
TEMPLATE   = app
CONFIG += c++11 link_pkgconfig
#LIBS += -L../widgets -lwidgets
PKGCONFIG      += dtkwidget dtkbase dtkutil

HEADERS    = \
    frame.h \
    pluginscontroller.h \
    framewidget.h \
    mainwidget.h

SOURCES    = \
            main.cpp \
    frame.cpp \
    pluginscontroller.cpp \
    framewidget.cpp \
    mainwidget.cpp

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
