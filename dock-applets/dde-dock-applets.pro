TEMPLATE = app

QT += quick qml core

SOURCES += \
    main.cpp

HEADERS += \
    resource.h

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
