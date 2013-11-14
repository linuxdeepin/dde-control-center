#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2011 ~ 2013 Deepin, Inc.
#               2011 ~ 2013 Wang YaoHua
#
# Author:     Wang YaoHua <mr.asianwang@gmail.com>
# Maintainer: Wang YaoHua <mr.asianwang@gmail.com>
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

from PyQt5.QtCore import pyqtSlot, pyqtSignal
from PyQt5.QtDBus import QDBusAbstractInterface, QDBusConnection, QDBusReply, QDBusMessage

class TestInterface(QDBusAbstractInterface):
    
    PropertiesChanged = pyqtSignal(QDBusMessage)
    # NewSignal = pyqtSignal(QDBusMessage)

    def __init__(self):
        # super(TestInterface, self).__init__("com.deepin.daemon.TestSignal", # service_name
        #                                     "/com/deepin/daemon/TestSignal", # path
        #                                     'com.deepin.daemon.TestSignal', # interface
        #                                     QDBusConnection.sessionBus(), None)

        super(TestInterface, self).__init__("com.deepin.daemon.Desktop", # service_name
                                            "/com/deepin/daemon/Desktop", # path
                                            'org.freedesktop.DBus.Properties', # interface
                                            QDBusConnection.sessionBus(), None)

    def getPropertyValue(self, interfaceName, propertyName):
        msg = self.call("Get", interfaceName, propertyName)
        reply = QDBusReply(msg) 
        return reply.value()
        
    def getShowComputerIcon(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "ShowComputerIcon")

@pyqtSlot(QDBusMessage)
def testSlot(msg):
    for arg in msg.arguments():
        print arg

# @pyqtSlot(QDBusMessage)
# def testSlot(d):
#     print d
#     print d.arguments()

if __name__ == "__main__":
    from PyQt5.QtCore import QCoreApplication

    app = QCoreApplication([])

    test = TestInterface()
    print test.getShowComputerIcon()
    test.PropertiesChanged.connect(testSlot)

    app.exec_()
