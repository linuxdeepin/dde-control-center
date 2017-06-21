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
    $$PWD/optionlistpage.h \
    $$PWD/comboboxwidget.h \
    $$PWD/filechoosewidget.h \
    $$PWD/separator.h \
    $$PWD/labels/tipslabel.h \
    $$PWD/spinboxwidget.h \
    $$PWD/titlevalueitem.h \
    $$PWD/plantextitem.h \
    $$PWD/basiclistview.h \
    $$PWD/basiclistmodel.h \
    $$PWD/basiclistdelegate.h \
    $$PWD/buttontuple.h \
    $$PWD/editablenextpagewidget.h \
    $$PWD/tipsitem.h

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
    $$PWD/optionlistpage.cpp \
    $$PWD/comboboxwidget.cpp \
    $$PWD/filechoosewidget.cpp \
    $$PWD/separator.cpp \
    $$PWD/labels/tipslabel.cpp \
    $$PWD/spinboxwidget.cpp \
    $$PWD/titlevalueitem.cpp \
    $$PWD/plantextitem.cpp \
    $$PWD/basiclistview.cpp \
    $$PWD/basiclistmodel.cpp \
    $$PWD/basiclistdelegate.cpp \
    $$PWD/buttontuple.cpp \
    $$PWD/editablenextpagewidget.cpp \
    $$PWD/tipsitem.cpp

RESOURCES += \
    $$PWD/resources.qrc

