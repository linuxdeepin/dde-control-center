TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
TARGET          = $$qtLibraryTarget(grub)
DESTDIR         = $$_PRO_FILE_PWD_/../

LIBS += -ldui

HEADERS += \
    mainwidget.h \
    shortcuts.h \
    setshortcutlist.h

SOURCES += \
    mainwidget.cpp \
    shortcuts.cpp \
    setshortcutlist.cpp
