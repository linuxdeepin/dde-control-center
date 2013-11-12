#!/usr/bin/env python

import os
import sys

from PyQt5.QtCore import QUrl, Qt
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQuick import QQuickView

root_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(root_path)

'''Start import modules class'''
from modules.system_info import Controller as system_info
from modules.system_update import Controller as system_update
'''End import modules class'''

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

'''Start modules context init'''
system_info_obj = system_info()
qml_context.setContextProperty('module_system_info', system_info_obj)

system_update_obj = system_update()
qml_context.setContextProperty('module_system_update', system_update_obj)
'''End modules context init'''

view.show()

sys.exit(app.exec_())
