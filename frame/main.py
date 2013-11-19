#!/usr/bin/env python

import os
import sys
import signal

from PyQt5.QtCore import QUrl, QTranslator, QObject
from PyQt5.QtGui import QGuiApplication

from dbus.mainloop.pyqt5 import DBusQtMainLoop
DBusQtMainLoop(set_as_default=True)

from Window import Window
from modules_info import ModulesId

root_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(root_path)

'''Start import modules class'''
from modules.system_info import Controller as module_system_info
from modules.system_update import Controller as module_system_update
from modules.power import Controller as module_power 
'''End import modules class'''

WIDTH = 360

def test(obj=None):
    print obj

if __name__ == '__main__':
    app = QGuiApplication(sys.argv)

    view = Window()
    #view.focusOutEvent.connect(test)
    view.setSource(QUrl.fromLocalFile(os.path.join(
        os.path.dirname(__file__), 'Main.qml')))
    view.engine().quit.connect(app.quit)

    screen_size = view.screen().size()
    view.setGeometry(screen_size.width() - 2,
            0, screen_size.width(), screen_size.height())

    qml_context = view.rootContext()
    qml_context.setContextProperty("windowView", view)
    qml_context.setContextProperty("screenSize", screen_size)
    modulesId = ModulesId()
    qml_context.setContextProperty("modulesId", modulesId)

    '''Start modules context init'''
    system_info_obj = module_system_info()
    qml_context.setContextProperty('module_system_info', system_info_obj)

    system_update_obj = module_system_update()
    qml_context.setContextProperty('module_system_update', system_update_obj)

    power_obj = module_power()
    qml_context.setContextProperty('module_power', power_obj)

    '''End modules context init'''

    trans = QTranslator(view)
    trans.load(os.path.join(root_path, 'locale', "translator_zh.qm"))
    app.installTranslator(trans)

    view.show()
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sys.exit(app.exec_())
