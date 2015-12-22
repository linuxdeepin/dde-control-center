include(./common.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = subdirs
SUBDIRS = widgets \
          frame \
          modules \
          dock-plugins

# Automating generation .qm files from .ts files
LANGUAGES = zh_CN

defineReplace(prependAll) {
    result =
    for (lang, $$1) {
        result ''= $$2$${lang}$$3
    }

    return ($$result)
}

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/translations/dde-control-center_, .ts)

qtPrepareTool(LRELEASE, lrelease)
for (tsfile, TRANSLATIONS) {
    qmfile = $$tsfile
    qmfile ~= s/\.ts/.qm/

    command = $$LRELEASE $$tsfile -qm $$qmfile
    system($$command) | error("Failed to execute: $$command")
}

# add install files
frame.depends = widgets
modules.depends = widgets

binary.path = $${PREFIX}/bin
binary.files = dde-control-center

desktop.path = $${PREFIX}/share/applications/
desktop.files = dde-control-center.desktop

service.path = $${PREFIX}/share/dbus-1/services
service.files = com.deepin.dde.ControlCenter.service

icons.path = $${PREFIX}/share/dde-control-center/modules/icons
icons.files = modules/icons/*

qm_files.path = $${PREFIX}/share/dde-control-center/translations/
qm_files.files = translations/*.qm

docs.path = /usr/share/dman/dde-control-center
docs.files = docs/*

INSTALLS = binary desktop service icons qm_files docs
