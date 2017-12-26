TEMPLATE = subdirs

SUBDIRS = navgation

!isEqual(DCC_DISABLE_PLUGIN_WEATHER, YES) {
    SUBDIRS += weather
}

!isEqual(DCC_DISABLE_PLUGIN_NOTIFY, YES) {
    SUBDIRS += notify
}
