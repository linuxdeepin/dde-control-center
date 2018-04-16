load(deepin_qt)

#include(./common.pri)
include(./config.pri)
#include(./modules/modules.pri)
#include(./interfaces/interfaces.pri)

TEMPLATE = subdirs
SUBDIRS  = \
            frame \
            dialogs

!isEqual(DISABLE_MAIN_PAGE, YES) {
    SUBDIRS += plugins
}

# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    !system($$PWD/translate_generation.sh): error("Failed to generate translation")
}

isEmpty(PREFIX) {
    PREFIX = /usr
}

# check sunway
isEqual(DCC_ENABLE_AUTOSTART, YES){
    autostart.path = /etc/xdg/autostart
    autostart.files = dde-control-center-autostart.desktop
    INSTALLS += autostart
}

# add install files
widgets.depends = helper
#frame.depends = widgets
#modules.depends = widgets

binary.path = $${PREFIX}/bin
binary.files = dde-control-center

plugins.path = $${PREFIX}/bin
plugins.files = modules/*.so

desktop.path = $${PREFIX}/share/applications/
desktop.files = dde-control-center.desktop

service.path = $${PREFIX}/share/dbus-1/services
service.files = com.deepin.dde.ControlCenter.service

qm_files.path = $${PREFIX}/share/dde-control-center/translations/
qm_files.files = translations/*.qm

docs.path = /usr/share/dman/dde-control-center
docs.files = docs/*

databases.path = $${PREFIX}/share/dde-control-center/
databases.files = data/timezone.db

TRANSLATIONS = translations/dde-control-center.ts

INSTALLS += binary desktop service qm_files docs databases

deepin_professional {
    host_x86_64: {
        addomain.path = /usr/share/polkit-1/actions
        addomain.files = com.deepin.controlcenter.addomain.policy
        INSTALLS += addomain
    }
}
