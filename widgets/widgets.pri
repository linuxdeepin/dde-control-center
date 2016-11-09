DEFINES += QT_MESSAGELOGCONTEXT
INCLUDEPATH += $$PWD

QT += widgets
PKGCONFIG   += dtkwidget

HEADERS += \
    $$PWD/settingsgroup.h \
    $$PWD/settingsitem.h \
    $$PWD/contentwidget.h \
    $$PWD/nextpagewidget.h \
    $$PWD/settingshead.h \
    $$PWD/switchwidget.h \
    $$PWD/timezoneitem.h \
    $$PWD/optionwidget.h \
    $$PWD/lineeditwidget.h

SOURCES += \
    $$PWD/settingsgroup.cpp \
#    $$PWD/main.cpp \
    $$PWD/settingsitem.cpp \
    $$PWD/contentwidget.cpp \
    $$PWD/nextpagewidget.cpp \
    $$PWD/settingshead.cpp \
    $$PWD/switchwidget.cpp \
    $$PWD/timezoneitem.cpp \
    $$PWD/optionwidget.cpp \
    $$PWD/lineeditwidget.cpp

RESOURCES += \
    $$PWD/dark/dcc_widgets_dark.qrc \
    $$PWD/light/dcc_widgets_light.qrc \
    $$PWD/UI/ui.qrc

