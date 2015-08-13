DESTDIR    = $$_PRO_FILE_PWD_/../

QT += widgets

include(private/private.pri)
INCLUDEPATH += $$PWD

HEADERS += $$PWD/searchlist.h \
    $$PWD/dynamiclabel.h \
    $$PWD/moduleheader.h \
    $$PWD/normallabel.h \
    $$PWD/highlightlabel.h
SOURCES += $$PWD/searchlist.cpp \
    $$PWD/dynamiclabel.cpp \
    $$PWD/moduleheader.cpp \
    $$PWD/normallabel.cpp \
    $$PWD/highlightlabel.cpp

RESOURCES += \
    $$PWD/themes/widgets_theme_dark.qrc \
    $$PWD/themes/widgets_theme_light.qrc
