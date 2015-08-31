TEMPLATE = subdirs
SUBDIRS = widgets \
          frame \
          modules \
          dock-plugins

frame.depends = widgets
modules.depends = widgets
