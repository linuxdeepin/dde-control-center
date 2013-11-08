#!/usr/bin/env python

import os
import sys

from PyQt5.QtCore import QUrl, Qt
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQuick import QQuickView

root_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(root_path)
import importlib

modules_dirs = ['update', 'system_info']

WIDTH = 360

app = QGuiApplication(sys.argv)

view = QQuickView()
view.setSource(QUrl.fromLocalFile(os.path.join(
    os.path.dirname(__file__), 'Main.qml')))
view.engine().quit.connect(app.quit)


view.setFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
view.setResizeMode(QQuickView.SizeRootObjectToView)

screen_size = view.screen().size()
view.setGeometry(screen_size.width() - WIDTH,
        0, WIDTH, screen_size.height() - 30)

qml_context = view.rootContext()
qml_context.setContextProperty("windowView", view)
qml_context.setContextProperty("screenSize", screen_size)

for module_dir in modules_dirs:
    try:
        mo = importlib.import_module('modules.%s' % module_dir)
        obj = mo.Controller()
        qml_context.setContextProperty(module_dir, obj)
    except:
        print "import Controller failed:", module_dir 

view.show()

sys.exit(app.exec_())
