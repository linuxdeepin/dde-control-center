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

from controlpanel import ControlPanel, DBusService
from constants import APP_DBUS_NAME, APP_OBJECT_PATH
from display_monitor import connect_to_primary_changed

from PyQt5.QtDBus import QDBusConnection, QDBusInterface
session_bus = QDBusConnection.sessionBus()

def main():
    root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    os.chdir(root_dir)

    panel = ControlPanel()
    panel.engine_obj.quit.connect(app.quit)
    connect_to_primary_changed(panel.display_primary_changed)

    if len(sys.argv) == 2:
        if sys.argv[1].endswith("/"):
            order = sys.argv[1][:-1]
        else:
            order = sys.argv[1]

        panel.view_object.showModule(order)

    panel.record_event.start()

    DBusService(panel)
    session_bus.registerObject(APP_OBJECT_PATH, panel)
    session_bus.registerService(APP_DBUS_NAME)

    sys.exit(app.exec_())

if __name__ == "__main__":
    iface = QDBusInterface(APP_DBUS_NAME, APP_OBJECT_PATH, '', session_bus)
    if iface.isValid():
        print "dss is running"
        iface.call("show")
        sys.exit(0)
    else:
        main()
