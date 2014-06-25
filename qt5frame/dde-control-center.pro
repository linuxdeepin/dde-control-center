TEMPLATE = app

QT += quick qml core widgets
SOURCES += main.cpp \
    popup.c \
    dwindow.cpp
RESOURCES += \
    views.qrc \
    modules.qrc

target.path = /usr/bin
INSTALLS += target

HEADERS += \
    popup.h \
    dwindow.h
