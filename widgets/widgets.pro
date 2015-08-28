QT += widgets
TEMPLATE = lib
CONFIG += c++11 staticlib link_pkgconfig
PKGCONFIG += dui

include(private/private.pri)

HEADERS += $$PWD/searchlist.h \
    $$PWD/dynamiclabel.h \
    $$PWD/moduleheader.h \
    $$PWD/normallabel.h \
    $$PWD/highlightlabel.h \
    $$PWD/imagenamebutton.h \
    $$PWD/addrmdoneline.h \
    $$PWD/multiaddcheckbutton.h \
    $$PWD/inputline.h \
    $$PWD/switchline.h \
    $$PWD/confirmbuttonline.h \
    $$PWD/applybuttonline.h \
    $$PWD/passwdline.h \
    $$PWD/generaladdbutton.h \
    $$PWD/generalremovebutton.h \
    $$PWD/multideletebutton.h \
    $$PWD/useravatar.h
SOURCES += $$PWD/searchlist.cpp \
    $$PWD/dynamiclabel.cpp \
    $$PWD/moduleheader.cpp \
    $$PWD/normallabel.cpp \
    $$PWD/highlightlabel.cpp \
    $$PWD/imagenamebutton.cpp \
    $$PWD/addrmdoneline.cpp \
    $$PWD/multiaddcheckbutton.cpp \
    $$PWD/inputline.cpp \
    $$PWD/switchline.cpp \
    $$PWD/confirmbuttonline.cpp \
    $$PWD/applybuttonline.cpp \
    $$PWD/passwdline.cpp \
    $$PWD/generaladdbutton.cpp \
    $$PWD/generalremovebutton.cpp \
    $$PWD/multideletebutton.cpp \
    $$PWD/useravatar.cpp

RESOURCES += \
    $$PWD/themes/widgets_theme_dark.qrc \
    $$PWD/themes/widgets_theme_light.qrc
