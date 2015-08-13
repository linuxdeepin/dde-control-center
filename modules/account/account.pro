TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../frame/
HEADERS         = \
    account.h \
    dbus/dbusaccount.h \
    dbus/dbusaccountuser.h \
    dbus/dbussessionmanager.h
SOURCES         = \
    account.cpp \
    dbus/dbusaccount.cpp \
    dbus/dbusaccountuser.cpp \
    dbus/dbussessionmanager.cpp
TARGET          = $$qtLibraryTarget(account)
DESTDIR         = $$_PRO_FILE_PWD_/../
