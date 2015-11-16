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
    networkbaseeditline.h \
    addconnectpage.h \
    adddslpage.h \
    addvpnpage.h \
    connecttohiddenappage.h \
    editlineinput.h \
    editlinecombobox.h \
    modemnetworklistitem.h \
    listwidgetcontainer.h \
    dtextaction.h \
    editlinemissingpackage.h \
    editlineswitchbutton.h \
    systemproxyline.h
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
    networkbaseeditline.cpp \
    addconnectpage.cpp \
    adddslpage.cpp \
    addvpnpage.cpp \
    connecttohiddenappage.cpp \
    editlineinput.cpp \
    editlinecombobox.cpp \
    modemnetworklistitem.cpp \
    listwidgetcontainer.cpp \
    dtextaction.cpp \
    editlinemissingpackage.cpp \
    editlineswitchbutton.cpp \
    systemproxyline.cpp
TARGET          = $$qtLibraryTarget(network)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += network.json

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target

RESOURCES += \
    theme.qrc
