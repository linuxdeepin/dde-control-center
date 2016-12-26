include(timezone_dialog.pri)

HEADERS += \
    $$PWD/datetime.h \
    $$PWD/clock.h \
    $$PWD/datesettings.h \
    $$PWD/datetimewidget.h \
    $$PWD/datetimemodule.h \
    $$PWD/datetimework.h \
    $$PWD/datetimemodel.h \
    $$PWD/timezoneitem.h \
    $$PWD/clockitem.h

SOURCES += \
    $$PWD/datetime.cpp \
    $$PWD/clock.cpp \
    $$PWD/datesettings.cpp \
    $$PWD/datetimewidget.cpp \
    $$PWD/datetimemodule.cpp \
    $$PWD/datetimework.cpp \
    $$PWD/datetimemodel.cpp \
    $$PWD/timezoneitem.cpp \
    $$PWD/clockitem.cpp

RESOURCES += \
    $$PWD/datetime.qrc

