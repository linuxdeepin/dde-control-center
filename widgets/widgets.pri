DESTDIR    = $$_PRO_FILE_PWD_/../

QT += widgets

include(private/private.pri)
INCLUDEPATH += $$PWD

HEADERS += $$PWD/searchlist.h \
    $$PWD/dynamiclabel.h
SOURCES += $$PWD/searchlist.cpp \
    $$PWD/dynamiclabel.cpp

RESOURCES += \
    $$PWD/widgetstheme.qrc
