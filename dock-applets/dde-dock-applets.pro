TEMPLATE = app

QT += quick qml core
SOURCES += main.cpp
RESOURCES += \
    qml.qrc \

icons.files = icons/*
icons.path = /usr/share/dde-control-center/dock-applets/icons

target.path = /usr/bin
INSTALLS += target icons

HEADERS += \
    resource.h
