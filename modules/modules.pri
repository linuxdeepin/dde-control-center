
include(accounts/accounts.pri)
include(display/display.pri)
include(datetime/datetime.pri)
include(power/power.pri)
include(update/update.pri)

HEADERS += \
    $$PWD/modulewidget.h

SOURCES += \
    $$PWD/modulewidget.cpp

INCLUDEPATH += $$PWD
