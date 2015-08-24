TEMPLATE        = lib
CONFIG         += plugin c++11 link_pkgconfig
QT             += widgets dbus
INCLUDEPATH    += ../../frame/ ../../widgets
PKGCONFIG += dui
LIBS += -L../../widgets -lwidgets

HEADERS        += datetime.h \
    dbus/dbustimedate.h \
    timewidget.h
SOURCES        += datetime.cpp \
    dbus/dbustimedate.cpp \
    timewidget.cpp
TARGET          = $$qtLibraryTarget(datetime)
DESTDIR         = $$_PRO_FILE_PWD_/../
DISTFILES      += datetime.json

RESOURCES += \
    theme.qrc
