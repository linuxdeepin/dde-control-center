#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2011~2013 Deepin, Inc.
#               2011~2013 Kaisheng Ye
#
# Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
# Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys
import signal
import threading

from PyQt5.QtCore import QUrl
from PyQt5.QtGui import QGuiApplication

from Window import Window
from modules_info import ModulesId
from event import RecordEvent
from unique_service import UniqueService
from nls import QtGettext

root_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(root_path)

WIDTH = 360

APP_DBUS_NAME = "com.deepin.system.settings"
APP_OBJECT_NAME = "/com/deepin/system/settings"

def unique_trigger():
    print "dss is running"

if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    uniqueService = UniqueService(APP_DBUS_NAME, APP_OBJECT_NAME)
    uniqueService.uniqueTrigger.connect(unique_trigger)

    view = Window()

    screen_size = view.screen().size()
    view.setGeometry(screen_size.width(),
            0, 360, screen_size.height())

    qml_context = view.rootContext()
    qml_context.setContextProperty("windowView", view)
    qml_context.setContextProperty("screenSize", screen_size)
    modulesId = ModulesId()
    qml_context.setContextProperty("modulesId", modulesId)
    qtgettext = QtGettext()
    qml_context.setContextProperty("qtgettext", qtgettext)

    #trans = QTranslator(view)
    #trans.load(os.path.join(root_path, 'locale', "zh_CN.qm"))
    #app.installTranslator(trans)

    view.setSource(QUrl.fromLocalFile(os.path.join(
        os.path.dirname(__file__), 'Main.qml')))
    view.engine().quit.connect(app.quit)
    view.show()

    view_object = view.rootObject()
    record_event = RecordEvent(view)
    record_event.enter_mouse_area.connect(view_object.displayTrayIcon)
    record_event.click_outer_area.connect(view_object.outerAreaClicked)
    
    thread = threading.Thread(target=record_event.filter_event)
    thread.setDaemon(True)
    thread.start()

    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sys.exit(app.exec_())
