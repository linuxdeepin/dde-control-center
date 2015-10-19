greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += widgets

TEMPLATE = lib
CONFIG += c++11 staticlib link_pkgconfig
PKGCONFIG += dui

include(widgets.pri)

HEADERS += \
    themes/dark/bluetoothlistitem.h

SOURCES += \
    themes/dark/bluetoothlistitem.cpp
