
CITIES_PATH=$$PWD/../../data/
DATABASE_RELEASE=$${PREFIX}/share/dde-control-center/

DEFINES += CITIES_DATABASE_PATH=\\\"$$CITIES_PATH\\\"
DEFINES += DATABASE_RELEASE_PATH=\\\"$$DATABASE_RELEASE\\\"

HEADERS += \
    $$PWD/datetime.h \
    $$PWD/datetimeutil.h \
    $$PWD/clock.h \
    $$PWD/datesettings.h \
    $$PWD/datetimewidget.h \
    $$PWD/datetimemodule.h \
    $$PWD/datetimework.h \
    $$PWD/datetimemodel.h \
    $$PWD/timezoneitem.h

SOURCES += \
    $$PWD/datetime.cpp \
    $$PWD/datetimeutil.cpp \
    $$PWD/clock.cpp \
    $$PWD/datesettings.cpp \
    $$PWD/datetimewidget.cpp \
    $$PWD/datetimemodule.cpp \
    $$PWD/datetimework.cpp \
    $$PWD/datetimemodel.cpp \
    $$PWD/timezoneitem.cpp

RESOURCES += \
    $$PWD/datetime.qrc

DISTFILES +=
