TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
TARGET          = $$qtLibraryTarget(grub)
DESTDIR         = $$_PRO_FILE_PWD_/../

LIBS += -ldui

HEADERS += \
    mainwidget.h \
    shortcuts.h \
    setshortcutlist.h \
    shortcutdbus.h

SOURCES += \
    mainwidget.cpp \
    shortcuts.cpp \
    setshortcutlist.cpp \
    shortcutdbus.cpp

RESOURCES += \
    theme.qrc
