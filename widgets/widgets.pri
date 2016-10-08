
DEFINES += QT_MESSAGELOGCONTEXT
INCLUDEPATH += $$PWD

QT += widgets
PKGCONFIG   += dtkwidget

HEADERS += \
    $$PWD/settingsgroup.h \
    $$PWD/settingsitem.h \
    $$PWD/contentwidget.h

SOURCES += \ 
    $$PWD/settingsgroup.cpp \
#    $$PWD/main.cpp \
    $$PWD/settingsitem.cpp \
    $$PWD/contentwidget.cpp

RESOURCES += \
    $$PWD/dark/dcc_widgets_dark.qrc \
    $$PWD/light/dcc_widgets_light.qrc
