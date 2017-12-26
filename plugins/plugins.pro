TEMPLATE = subdirs

SUBDIRS = navgation

isEqual(DCC_PLUGIN_WEATHER, YES) {
    SUBDIRS += weather
}

isEqual(DCC_PLUGIN_NOTIFY, YES) {
    SUBDIRS += notify
}
