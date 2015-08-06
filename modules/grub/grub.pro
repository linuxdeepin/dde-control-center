TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS        += grub.h \
                grubwidget.h \
    #grubdbus.h \
    grubbackground.h \
    dbustheme.h \
    dbusgrub.h
SOURCES        += grub.cpp \
                grubwidget.cpp \
    #grubdbus.cpp \
    grubbackground.cpp \
    dbustheme.cpp \
    dbusgrub.cpp
TARGET          = $$qtLibraryTarget(grub)
DESTDIR         = $$_PRO_FILE_PWD_/../

LIBS += -ldui
