TEMPLATE = subdirs
SUBDIRS = frame \
          widgets \
          modules

modules.depends = widgets
