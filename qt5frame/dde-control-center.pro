TEMPLATE = app

QT += quick qml core widgets dbus
SOURCES += main.cpp \
    qmlloader.cpp
RESOURCES += \
    views.qrc \
    modules.qrc

target.path = /usr/bin
INSTALLS += target

HEADERS += \
    qmlloader.h
