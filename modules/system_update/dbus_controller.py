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

from PyQt5.QtCore import QObject, pyqtSlot, pyqtSignal
from PyQt5.QtWidgets import QApplication
from PyQt5.QtDBus import QDBusAbstractInterface, QDBusConnection, QDBusReply

class DscInterface(QDBusAbstractInterface):
    update_signal = pyqtSignal(str)

    def __init__(self, service, path, connection, parent=None):
        super(DscInterface, self).__init__(service, path,
                'com.linuxdeepin.softwarecenter', connection, parent)

    def get_download_size(self, name):
        result = QDBusReply(self.call('get_download_size', name)).value()
        return json.dumps(result)


class Controller(QObject):

    def __init__(self, parent=None):
        QObject.__init__(self)
        self.dsc_interface = DscInterface('com.linuxdeepin.softwarecenter', '/com/linuxdeepin/softwarecenter',
                QDBusConnection.systemBus(), self)
        self.dsc_interface.update_signal.connect(self.message_handler)

    def message_handler(self, *args):
        print self, args

    @pyqtSlot(str, result=str)
    def get_download_size(self, name):
        return self.dsc_interface.get_download_size(name)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    conn = Controller()
    msg = conn.get_download_size("deepin-software-center-data")
