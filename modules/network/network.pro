include(../../common.pri)

TEMPLATE        = lib
CONFIG         += plugin  c++11 link_pkgconfig
QT             += widgets svg
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS         += network.h \
    networkmainwidget.h \
    dbus/dbusnetwork.h \
    dbus/dbusconnectionsession.h \
    wirednetworklistitem.h \
    wirelessnetworklistitem.h \
    vpnconnectswidget.h \
    systemproxywidget.h \
    abstractdevicewidget.h \
    networkgenericlistitem.h \
    networkglobal.h \
    inputpassworddialog.h \
    connecttohiddenapwidget.h \
    networkbaseeditline.h
SOURCES         += network.cpp \
    networkmainwidget.cpp \
    dbus/dbusnetwork.cpp \
    dbus/dbusconnectionsession.cpp \
    wirednetworklistitem.cpp \
    wirelessnetworklistitem.cpp \
    vpnconnectswidget.cpp \
    systemproxywidget.cpp \
    abstractdevicewidget.cpp \
    networkgenericlistitem.cpp \
    inputpassworddialog.cpp \
    connecttohiddenapwidget.cpp \
    networkbaseeditline.cpp
TARGET          = $$qtLibraryTarget(network)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += network.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

RESOURCES += \
    theme.qrc
