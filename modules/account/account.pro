include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets multimedia multimediawidgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui

CONFIG(debug) {
    include(../../widgets/widgets.pri)
} else {
    LIBS += -L../../widgets -lwidgets
}

HEADERS         += \
    account.h \
    dbus/dbusaccount.h \
    dbus/dbusaccountuser.h \
    dbus/dbussessionmanager.h \
    ui/accountmainwidget.h \
    ui/user_list/avatargrid.h \
    ui/user_list/passwordframe.h \
    ui/user_list/userexpand.h \
    ui/user_list/userlistpanel.h \
    ui/user_list/usernametitle.h \
    ui/user_list/webcamavatarpanel.h \
    ui/user_create/createuserpanel.h \
    ui/user_list/userexpandcontent.h \
    ui/user_list/userexpandheader.h \
    ui/controlline.h

SOURCES         += \
    account.cpp \
    dbus/dbusaccount.cpp \
    dbus/dbusaccountuser.cpp \
    dbus/dbussessionmanager.cpp \
    ui/accountmainwidget.cpp \
    ui/user_list/avatargrid.cpp \
    ui/user_list/passwordframe.cpp \
    ui/user_list/userexpand.cpp \
    ui/user_list/userlistpanel.cpp \
    ui/user_list/usernametitle.cpp \
    ui/user_list/webcamavatarpanel.cpp \
    ui/user_create/createuserpanel.cpp \
    ui/user_list/userexpandcontent.cpp \
    ui/user_list/userexpandheader.cpp \
    ui/controlline.cpp

TARGET          = $$qtLibraryTarget(account)
DESTDIR         = $$_PRO_FILE_PWD_/../

DISTFILES += account.json
RESOURCES += \
    theme.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
