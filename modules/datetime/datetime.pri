
CITIES_PATH=$$PWD/../../data/timezone.db
DEFINES += CITIES_DATABASE_PATH=\\\"$$CITIES_PATH\\\"

HEADERS += \
    $$PWD/datetime.h \
    $$PWD/datetimeutil.h \
    $$PWD/clock.h

SOURCES += \
    $$PWD/datetime.cpp \
    $$PWD/datetimeutil.cpp \
    $$PWD/clock.cpp
