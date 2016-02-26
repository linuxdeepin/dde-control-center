QT += widgets

TEMPLATE = lib
CONFIG += c++11 staticlib link_pkgconfig
PKGCONFIG += gtk+-2.0
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/helper.h

SOURCES += \
    $$PWD/helper.cpp
