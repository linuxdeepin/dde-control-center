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
    $$PWD/lineeditwidget.h \
    $$PWD/labels/normallabel.h \
    $$PWD/labels/largelabel.h \
    $$PWD/labels/smalllabel.h \
    $$PWD/translucentframe.h \
    $$PWD/searchinput.h \
    $$PWD/dccslider.h \
    $$PWD/settingsheaderitem.h \
    $$PWD/titledslideritem.h \
    $$PWD/dccsliderannotated.h \
    $$PWD/optionitem.h \
    $$PWD/nextbutton.h \
    $$PWD/backbutton.h \
    $$PWD/loadingindicator.h \
    $$PWD/optionlistpage.h

SOURCES += \
    $$PWD/settingsgroup.cpp \
#    $$PWD/main.cpp \
    $$PWD/settingsitem.cpp \
    $$PWD/contentwidget.cpp \
    $$PWD/nextpagewidget.cpp \
    $$PWD/settingshead.cpp \
    $$PWD/switchwidget.cpp \
    $$PWD/lineeditwidget.cpp \
    $$PWD/labels/normallabel.cpp \
    $$PWD/labels/largelabel.cpp \
    $$PWD/labels/smalllabel.cpp \
    $$PWD/translucentframe.cpp \
    $$PWD/searchinput.cpp \
    $$PWD/dccslider.cpp \
    $$PWD/settingsheaderitem.cpp \
    $$PWD/titledslideritem.cpp \
    $$PWD/dccsliderannotated.cpp \
    $$PWD/optionitem.cpp \
    $$PWD/nextbutton.cpp \
    $$PWD/backbutton.cpp \
    $$PWD/loadingindicator.cpp \
    $$PWD/optionlistpage.cpp

RESOURCES += \
    $$PWD/resources.qrc

