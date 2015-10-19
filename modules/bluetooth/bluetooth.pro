include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui

CONFIG(debug) {
    include(../../widgets/widgets.pri)
} else {
    LIBS += -L../../widgets -lwidgets
}

QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         += bluetooth.h \
    bluetoothmainwidget.h
SOURCES         += bluetooth.cpp \
    bluetoothmainwidget.cpp
TARGET          = $$qtLibraryTarget(bluetooth)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += bluetooth.json

RESOURCES += \
    theme.qrc

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target
