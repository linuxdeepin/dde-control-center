TEMPLATE = subdirs
SUBDIRS = frame \
          widgets \
          modules \
          dock-plugins

modules.depends = widgets
