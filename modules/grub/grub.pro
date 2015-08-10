TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS        += grub.h \
                grubwidget.h \
    grubbackground.h \
    dbustheme.h \
    dbusgrub.h
SOURCES        += grub.cpp \
                grubwidget.cpp \
    grubbackground.cpp \
    dbustheme.cpp \
    dbusgrub.cpp
TARGET          = $$qtLibraryTarget(grub)
DESTDIR         = $$_PRO_FILE_PWD_/../

LIBS += -ldui
