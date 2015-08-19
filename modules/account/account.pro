include(../../widgets/widgets.pri)

TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets
INCLUDEPATH    += ../../frame/
LIBS += -ldui

HEADERS         += \
    account.h \
    dbus/dbusaccount.h \
    dbus/dbusaccountuser.h \
    dbus/dbussessionmanager.h \
    UserContent/usercontent.h \
    userexpand.h \
    userheader.h \
    userlistpanel.h \
    usernametitle.h \
    usericon.h \
    UserContent/webcamavatarpanel.h \
    UserContent/avatargrid.h \
    accountmainwidget.h \
    UserContent/switchline.h \
    UserContent/accounttypeline.h \
    UserContent/passwordline.h
SOURCES         += \
    account.cpp \
    dbus/dbusaccount.cpp \
    dbus/dbusaccountuser.cpp \
    dbus/dbussessionmanager.cpp \
    UserContent/usercontent.cpp \
    userexpand.cpp \
    userheader.cpp \
    userlistpanel.cpp \
    usernametitle.cpp \
    usericon.cpp \
    UserContent/webcamavatarpanel.cpp \
    UserContent/avatargrid.cpp \
    accountmainwidget.cpp \
    UserContent/switchline.cpp \
    UserContent/accounttypeline.cpp \
    UserContent/passwordline.cpp
TARGET          = $$qtLibraryTarget(account)
DESTDIR         = $$_PRO_FILE_PWD_/../

RESOURCES += \
    theme.qrc
