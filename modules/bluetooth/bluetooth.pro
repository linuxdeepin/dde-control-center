include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

QT             += widgets
HEADERS         += bluetooth.h \
    bluetoothmainwidget.h \
    bluetoothlistitem.h
SOURCES         += bluetooth.cpp \
    bluetoothmainwidget.cpp \
    bluetoothlistitem.cpp
TARGET          = $$qtLibraryTarget(bluetooth)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += bluetooth.json

RESOURCES += \
    theme.qrc

target.path = $${PREFIX}/share/dde-control-center/modules/
INSTALLS += target
