greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += widgets

TEMPLATE = lib
CONFIG += c++11 staticlib link_pkgconfig
PKGCONFIG += dtkbase dtkwidget

include(widgets.pri)
