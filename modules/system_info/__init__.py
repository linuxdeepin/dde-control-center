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
import signal

from PyQt5.QtCore import QObject, pyqtSlot, QCoreApplication
from PyQt5.QtDBus import QDBusInterface, QDBusConnection, QDBusReply, QDBusVariant

class Interface(QDBusInterface):
    def __init__(self, service, path, connection, parent=None):
        super(Interface, self).__init__(service, path,
                'org.freedesktop.DBus.Properties', connection, parent)

    def get_systeminfo(self):
        msg = self.call('GetAll', 'com.deepin.daemon.SystemInfo')
        result = QDBusReply(msg).value()
        return json.dumps(result)


class Controller(QObject):
    def __init__(self, parent=None):
        QObject.__init__(self)
        self.interface = Interface('com.deepin.daemon.SystemInfo', '/com/deepin/daemon/SystemInfo',
                QDBusConnection.sessionBus(), self)
        QDBusConnection.sessionBus().connect('com.deepin.daemon.SystemInfo', 
                '/com/deepin/daemon/SystemInfo',
                'org.freedesktop.DBus.Properties',
                'PropertiesChanged', self.messageSlot)

    @pyqtSlot(str, QObject, QObject)
    def messageSlot(self, *args):
        print args

    @pyqtSlot(result=str)
    def get_systeminfo(self):
        return self.interface.get_systeminfo()

    #@pyqtSlot(str, result=bool)
    def set_processor(self, name):
        return QDBusReply(self.interface.call("Set", "com.deepin.daemon.SystemInfo", "Processor", QDBusVariant(name)))

if __name__ == '__main__':
    app = QCoreApplication(sys.argv)
    c = Controller()
    msg = c.set_processor("Intel abc")
    if not msg.isValid():
        print msg.error().message()

    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sys.exit(app.exec_())
