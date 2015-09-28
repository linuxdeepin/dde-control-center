include(./common.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = subdirs
SUBDIRS = widgets \
          frame \
          modules \
          dock-plugins

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

INSTALLS = binary desktop service icons
