#! /usr/bin/env python
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

import sys
import json

from PyQt5.QtCore import QObject, pyqtSlot
from PyQt5.QtDBus import QDBusInterface, QDBusConnection, QDBusReply
from PyQt5.QtGui import QGuiApplication

properties_iface_name = 'org.freedesktop.DBus.Properties'

service_name = 'com.deepin.daemon.SystemInfo'
object_path = '/com/deepin/daemon/SystemInfo'
iface_name = 'com.deepin.daemon.SystemInfo'

class Controller(QObject):
    def __init__(self, parent=None):
        QObject.__init__(self)
        self.interface = QDBusInterface(
                service_name,
                object_path,
                properties_iface_name,
                QDBusConnection.sessionBus(),
                self)

    @pyqtSlot(result=str)
    def get_systeminfo(self):
        result = QDBusReply(self.interface.call('GetAll', 'com.deepin.daemon.SystemInfo')).value()
        return json.dumps(result)

if __name__ == '__main__':
    app = QGuiApplication(sys.argv)
    controller = Controller()
    print controller.get_systeminfo()
