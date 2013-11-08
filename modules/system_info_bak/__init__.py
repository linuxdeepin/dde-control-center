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

import json

from PyQt5.QtCore import QObject, pyqtSlot
from PyQt5.QtDBus import QDBusAbstractInterface, QDBusConnection, QDBusReply

class Interface(QDBusAbstractInterface):
    def __init__(self, service, path, connection, parent=None):
        super(Interface, self).__init__(service, path,
                'com.deepin.daemon.systeminfo', connection, parent)

    def get_systeminfo(self):
        msg = self.call("GetSystemInfo")
        result = QDBusReply(msg).value()
        return json.dumps(result)


class Controller(QObject):
    def __init__(self, parent=None):
        QObject.__init__(self)
        self.interface = Interface('com.deepin.daemon.systeminfo', '/com/deepin/daemon/systeminfo',
                QDBusConnection.sessionBus(), self)

    @pyqtSlot(result=str)
    def get_systeminfo(self):
        return self.interface.get_systeminfo()
