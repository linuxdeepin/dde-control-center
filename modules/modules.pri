
include(accounts/accounts.pri)
include(display/display.pri)
include(datetime/datetime.pri)
include(power/power.pri)
include(update/update.pri)
include(defapp/defapp.pri)
include(sound/sound.pri)
include(bluetooth/bluetooth.pri)

HEADERS += \
    $$PWD/modulewidget.h \
    $$PWD/moduleworker.h

SOURCES += \
    $$PWD/modulewidget.cpp

INCLUDEPATH += $$PWD
