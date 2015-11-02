include(../../common.pri)
TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

TARGET          = $$qtLibraryTarget(shortcuts)
DESTDIR         = $$_PRO_FILE_PWD_/../

HEADERS += \
    mainwidget.h \
    shortcuts.h \
    shortcutwidget.h \
    tooltip.h \
    selectdialog.h \
    shortcutdbus.h \
    shortcutedit.h

SOURCES += \
    mainwidget.cpp \
    shortcuts.cpp \
    shortcutwidget.cpp \
    tooltip.cpp \
    selectdialog.cpp \
    shortcutdbus.cpp \
    shortcutedit.cpp

DISTFILES += shortcuts.json
RESOURCES += \
    theme.qrc

target.path = $${PREFIX}/lib/dde-control-center/modules/
INSTALLS += target
