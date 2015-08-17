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
    accountexpand.h \
    mainwidget.h \
    accountheader.h \
    accounticon.h \
    accountlistpanel.h \
    accountcontent.h \
    accountnametitle.h
SOURCES         += \
    account.cpp \
    dbus/dbusaccount.cpp \
    dbus/dbusaccountuser.cpp \
    dbus/dbussessionmanager.cpp \
    accountexpand.cpp \
    mainwidget.cpp \
    accountheader.cpp \
    accounticon.cpp \
    accountlistpanel.cpp \
    accountcontent.cpp \
    accountnametitle.cpp
TARGET          = $$qtLibraryTarget(account)
DESTDIR         = $$_PRO_FILE_PWD_/../
