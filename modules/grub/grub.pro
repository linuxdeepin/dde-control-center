TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
HEADERS        += grub.h \
                grubwidget.h \
    grubdbus.h \
    grubbackground.h
SOURCES        += grub.cpp \
                grubwidget.cpp \
    grubdbus.cpp \
    grubbackground.cpp
TARGET          = $$qtLibraryTarget(grub)
DESTDIR         = $$_PRO_FILE_PWD_/../

LIBS += -ldui
