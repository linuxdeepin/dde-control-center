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
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

from controlpanel import ControlPanel
from unique_service import UniqueService
from constants import APP_DBUS_NAME, APP_OBJECT_PATH

from PyQt5.QtWidgets import QApplication

def main():
    root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    os.chdir(root_dir)

    app = QApplication(sys.argv)
    uniqueService = UniqueService(APP_DBUS_NAME, APP_OBJECT_PATH)
    uniqueService.uniqueTrigger.connect(unique_trigger)

    panel = ControlPanel()
    panel.engine_obj.quit.connect(app.quit)
    panel.show()

    if len(sys.argv) == 2:
        if sys.argv[1] in panel.modulesId._l18n_names.keys():
            panel.view_object.showModule(sys.argv[1])
        else:
            print "Error module id:", sys.argv[1]
    
    panel.record_event.start()

    sys.exit(app.exec_())

def unique_trigger():
    print "dss is running"

if __name__ == "__main__":
    main()
