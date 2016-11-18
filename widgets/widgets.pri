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
    $$PWD/optionwidget.h \
    $$PWD/lineeditwidget.h \
    $$PWD/labels/normallabel.h \
    $$PWD/labels/largelabel.h \
    $$PWD/labels/smalllabel.h \
    $$PWD/translucentframe.h

SOURCES += \
    $$PWD/settingsgroup.cpp \
#    $$PWD/main.cpp \
    $$PWD/settingsitem.cpp \
    $$PWD/contentwidget.cpp \
    $$PWD/nextpagewidget.cpp \
    $$PWD/settingshead.cpp \
    $$PWD/switchwidget.cpp \
    $$PWD/optionwidget.cpp \
    $$PWD/lineeditwidget.cpp \
    $$PWD/labels/normallabel.cpp \
    $$PWD/labels/largelabel.cpp \
    $$PWD/labels/smalllabel.cpp \
    $$PWD/translucentframe.cpp

RESOURCES += \
    $$PWD/themes/dark/dcc_widgets_dark.qrc \
    $$PWD/themes/light/dcc_widgets_light.qrc \
    $$PWD/themes/common/dcc_widgets_common.qrc \
    $$PWD/UI/ui.qrc

