TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS        += grub.h \
                grubwidget.h \
    grubbackground.h \
    dbustheme.h \
    dbusgrub.h \
    bootmenulist.h
SOURCES        += grub.cpp \
                grubwidget.cpp \
    grubbackground.cpp \
    dbustheme.cpp \
    dbusgrub.cpp \
    bootmenulist.cpp
TARGET          = $$qtLibraryTarget(grub)
DESTDIR         = $$_PRO_FILE_PWD_/../

RESOURCES += \
    theme.qrc
