TEMPLATE = app

QT += quick qml core dbus

SOURCES += \
    main.cpp \
    qmlloader.cpp \
    helper.cpp

HEADERS += \
    qmlloader.h \
    helper.h

RESOURCES += \
    qml.qrc \

#VARIABLES
isEmpty(PREFIX) {
    PREFIX = /usr
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
DOCKAPPLETSDIR = $$DATADIR/dde-control-center/dock-applets

DEFINES += DATADIR=\\\"$$DATADIR\\\" DOCKAPPLETSDIR=\\\"$$DOCKAPPLETSDIR\\\"

#MAKE INSTALL

target.path =$$BINDIR

icons.files = icons/*
icons.path = $$DOCKAPPLETSDIR/icons

INSTALLS += target icons
