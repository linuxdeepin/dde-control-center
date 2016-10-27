
CITIES_PATH=$$PWD/../../data/timezone.db
DEFINES += CITIES_DATABASE_PATH=\\\"$$CITIES_PATH\\\"

HEADERS += \
    $$PWD/datetime.h \
    $$PWD/datetimeutil.h \
    $$PWD/clock.h \
    $$PWD/datesettings.h \
    $$PWD/datetimewidget.h \
    $$PWD/timezonehead.h

SOURCES += \
    $$PWD/datetime.cpp \
    $$PWD/datetimeutil.cpp \
    $$PWD/clock.cpp \
    $$PWD/datesettings.cpp \
    $$PWD/datetimewidget.cpp \
    $$PWD/timezonehead.cpp

RESOURCES += \
    $$PWD/datetime.qrc

DISTFILES +=
