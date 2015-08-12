TEMPLATE        = lib
CONFIG         += plugin c++11
QT             += widgets dbus
INCLUDEPATH    += ../../frame/
TARGET          = $$qtLibraryTarget(shortcuts)
DESTDIR         = $$_PRO_FILE_PWD_/../

LIBS += -ldui
include(../../widgets/widgets.pri)

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
