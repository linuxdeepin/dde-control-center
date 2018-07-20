TEMPLATE = subdirs

SUBDIRS = navigation

load(deepin_qt)

deepin_desktop {
    !isEqual(DCC_DISABLE_PLUGIN_WEATHER, YES) {
        SUBDIRS += weather
    }

    !isEqual(DCC_DISABLE_PLUGIN_NOTIFY, YES) {
        SUBDIRS += notify
    }
}

