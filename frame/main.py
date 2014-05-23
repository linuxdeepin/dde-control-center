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
from PyQt5 import QtCore
from PyQt5.QtCore import QCoreApplication
if os.name == 'posix':
    QCoreApplication.setAttribute(QtCore.Qt.AA_X11InitThreads, True)

import sys
from PyQt5.QtWidgets import QApplication
app = QApplication(sys.argv)

import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

from control_panel import ControlPanel
from constants import APP_DBUS_NAME, APP_OBJECT_PATH

from PyQt5 import QtDBus
session_bus = QtDBus.QDBusConnection.sessionBus()

from PyQt5 import QtQml
from popup import PopupItem

QtQml.qmlRegisterType(PopupItem, "DGui", 1, 0, "PopupItem")


def main():
    success = session_bus.registerService(APP_DBUS_NAME)
    if not success:
        print 'dss is running...'
        if len(sys.argv) == 2:
            iface = QtDBus.QDBusInterface(APP_DBUS_NAME, APP_OBJECT_PATH, "", session_bus)
            if sys.argv[1] == "show":
                iface.call("Show")
            elif sys.argv[1] == "toggle":
                iface.call("Toggle")
            else:
                iface.call("ShowModule", sys.argv[1])
        sys.exit(0)

    root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    os.chdir(root_dir)

    panel = ControlPanel()
    panel.engine.quit.connect(app.quit)

    if len(sys.argv) > 1:
        if sys.argv[1].endswith("/"):
            order = sys.argv[1][:-1]
        else:
            order = sys.argv[1]

        if order == "show":
            if len(sys.argv) == 3:
                second = int(sys.argv[2])
            else:
                second = 0
            panel.view_object.showDss(second)
        else:
            panel.view_object.showModule(order)

    session_bus.registerObject(APP_OBJECT_PATH, panel)
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
