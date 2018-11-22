
!isEqual(DISABLE_BLUETOOTH, YES) {
    include(bluetooth/bluetooth.pri)
}

!isEqual(DISABLE_DISPLAY, YES) {
    include(display/display.pri)
}

!isEqual(DISABLE_ACCOUNT, YES) {
    include(accounts/accounts.pri)
}

!isEqual(DISABLE_DATETIME, YES) {
    include(datetime/datetime.pri)
}

!isEqual(DISABLE_POWER, YES) {
    include(power/power.pri)
}

!isEqual(DISABLE_SYS_UPDATE, YES) {
    include(update/update.pri)
}

!isEqual(DISABLE_DEFAULT_APPLICATIONS, YES) {
    include(defapp/defapp.pri)
}

!isEqual(DISABLE_SOUND, YES) {
    include(sound/sound.pri)
}

!isEqual(DISABLE_KEYBOARD, YES) {
    include(keyboard/keyboard.pri)
}

!isEqual(DISABLE_MOUSE, YES) {
    include(mouse/mouse.pri)
}

!isEqual(DISABLE_WACOM, YES) {
    include(wacom/wacom.pri)
}

!isEqual(DISABLE_PERSONALIZATION, YES) {
    include(personalization/personalization.pri)
}

!isEqual(DISABLE_SYSINFO, YES) {
    include(systeminfo/systeminfo.pri)
}

include(network/network.pri)

HEADERS += \
    $$PWD/modulewidget.h \
    $$PWD/moduleworker.h \
    $$PWD/modulewidgetheader.h

SOURCES += \
    $$PWD/modulewidget.cpp \
    $$PWD/modulewidgetheader.cpp

INCLUDEPATH += $$PWD
