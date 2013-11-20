#!/usr/bin/env python

import os
import sys
import signal
import threading

from PyQt5.QtCore import QUrl, QTranslator
from PyQt5.QtGui import QGuiApplication

from Window import Window
from modules_info import ModulesId
from event import RecordEvent
from unique_service import UniqueService


root_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(root_path)

WIDTH = 360

APP_DBUS_NAME = "com.deepin.system.settings"
APP_OBJECT_NAME = "/com/deepin/system/settings"

if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    uniqueService = UniqueService(APP_DBUS_NAME, APP_OBJECT_NAME)

    view = Window()
    view.setSource(QUrl.fromLocalFile(os.path.join(
        os.path.dirname(__file__), 'Main.qml')))
    view.engine().quit.connect(app.quit)

    screen_size = view.screen().size()
    view.setGeometry(screen_size.width(),
            0, 360, screen_size.height())

    qml_context = view.rootContext()
    qml_context.setContextProperty("windowView", view)
    qml_context.setContextProperty("screenSize", screen_size)
    modulesId = ModulesId()
    qml_context.setContextProperty("modulesId", modulesId)

    trans = QTranslator(view)
    trans.load(os.path.join(root_path, 'locale', "translator_zh.qm"))
    app.installTranslator(trans)

    view.show()

    record_event = RecordEvent(view)
    
    thread = threading.Thread(target=record_event.filter_event)
    thread.setDaemon(True)
    thread.start()

    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sys.exit(app.exec_())
