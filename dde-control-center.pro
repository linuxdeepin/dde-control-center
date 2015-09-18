TEMPLATE = subdirs
SUBDIRS = widgets \
          frame \
          modules \
          dock-plugins

frame.depends = widgets
modules.depends = widgets

isEmpty(PREFIX){
    PREFIX=/usr/local
} 

binary.path = $${PREFIX}/bin
binary.files = dde-control-center

desktop.path = $${PREFIX}/share/applications/
desktop.files = dde-control-center.desktop

service.path = $${PREFIX}/share/dbus-1/services
service.files = com.deepin.dde.ControlCenter.service

modules.path = $${PREFIX}/share/dde-control-center/modules
modules.files = modules/*.so

icons.path = $${PREFIX}/share/dde-control-center/modules/icons
icons.files = modules/icons/*

dock-plugins.path = $${PREFIX}/share/dde-dock/plugins
dock-plugins.files = dock-plugins/*.so

INSTALLS = binary desktop service modules dock-plugins icons
