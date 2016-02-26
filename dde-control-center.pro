include(./common.pri)

TEMPLATE = subdirs
SUBDIRS = widgets \
          frame \
          modules \
          helper \
          dock-plugins

# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    system($$PWD/translate_generation.sh)
}


# add install files
widgets.depends = helper
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
