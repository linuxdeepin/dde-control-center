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
    shortcutwidget.h \
    tooltip.h \
    selectdialog.h

SOURCES += \
    mainwidget.cpp \
    shortcuts.cpp \
    shortcutwidget.cpp \
    tooltip.cpp \
    selectdialog.cpp

RESOURCES += \
    theme.qrc
