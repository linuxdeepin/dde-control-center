QT += widgets dbus

HEADERS    = frame.h \
             interfaces.h \
    homescreen.h \
    modulemetadata.h \
    constants.h \
    sidebar.h \
    contentview.h \
    dtipsframe.h \
    dbus/dbuscontrolcenter.h \
    dbus/dbususer.h \
    dbus/dbusaccounts.h
SOURCES    = frame.cpp \
             main.cpp \
    homescreen.cpp \
    sidebar.cpp \
    contentview.cpp \
    dtipsframe.cpp \
    dbus/dbuscontrolcenter.cpp \
    dbus/dbususer.cpp \
    dbus/dbusaccounts.cpp

include(../cutelogger/cutelogger.pri)
TARGET     = dde-control-center
DESTDIR    = $$_PRO_FILE_PWD_/../

CONFIG += c++11

LIBS += -ldui -L../widgets -lwidgets

RESOURCES += \
    qss.qrc \
    images.qrc

INCLUDEPATH    += ../widgets
