QT += widgets dbus svg

HEADERS    = \
    frame.h

SOURCES    = \
            main.cpp \
    frame.cpp

TARGET     = dde-control-center
DESTDIR    = $$_PRO_FILE_PWD_/../

CONFIG += c++11 link_pkgconfig
#LIBS += -L../widgets -lwidgets
PKGCONFIG      += dtkwidget dtkbase dtkutil

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
