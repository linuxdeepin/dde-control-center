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
    mainwidget.h \
    UserContent/usercontent.h \
    userexpand.h \
    userheader.h \
    userlistpanel.h \
    usernametitle.h \
    usericon.h \
    UserContent/allavatarpanel.h \
    UserContent/historyavatarpanel.h \
    UserContent/webcamavatarpanel.h \
    UserContent/avatargrid.h
SOURCES         += \
    account.cpp \
    dbus/dbusaccount.cpp \
    dbus/dbusaccountuser.cpp \
    dbus/dbussessionmanager.cpp \
    mainwidget.cpp \
    UserContent/usercontent.cpp \
    userexpand.cpp \
    userheader.cpp \
    userlistpanel.cpp \
    usernametitle.cpp \
    usericon.cpp \
    UserContent/allavatarpanel.cpp \
    UserContent/historyavatarpanel.cpp \
    UserContent/webcamavatarpanel.cpp \
    UserContent/avatargrid.cpp
TARGET          = $$qtLibraryTarget(account)
DESTDIR         = $$_PRO_FILE_PWD_/../
