
CITIES_PATH=$$PWD/../../data/
DEFINES += CITIES_DATABASE_PATH=\\\"$$CITIES_PATH\\\"

HEADERS += \
    $$PWD/datetime.h \
    $$PWD/datetimeutil.h \
    $$PWD/clock.h \
    $$PWD/datesettings.h \
    $$PWD/datetimewidget.h \
    $$PWD/timezonehead.h \
    $$PWD/datetimemodule.h \
    $$PWD/datetimework.h \
    $$PWD/chosedialog.h \
    $$PWD/datetimemodel.h

SOURCES += \
    $$PWD/datetime.cpp \
    $$PWD/datetimeutil.cpp \
    $$PWD/clock.cpp \
    $$PWD/datesettings.cpp \
    $$PWD/datetimewidget.cpp \
    $$PWD/timezonehead.cpp \
    $$PWD/datetimemodule.cpp \
    $$PWD/datetimework.cpp \
    $$PWD/chosedialog.cpp \
    $$PWD/datetimemodel.cpp

RESOURCES += \
    $$PWD/datetime.qrc

DISTFILES +=
