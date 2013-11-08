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

from PyQt5.QtCore import QObject, pyqtSlot
from PyQt5.QtDBus import QtDBusAbstractInterface, QDBusConnection

class SystemInfoInterface(QtDBusAbstractInterface):
    
    def __init__(self):
        super(SystemInfoInterface, self).__init__("com.deepin.daemon.systeminfo", # service_name
                                                  "com/deepin/daemon/systeminfo", # path
                                                  'com.deepin.daemon.systeminfo', # interface
                                                  QDBusConnection.SessionBus(), None)
    def getSystemInfo(self):
        return self.call("GetSystemInfo")
    
class IfaceWrapper(QObject):
    def __init__(self, systemInfoIface):
        self.systemInfoIface = systemInfoIface
        
    @pyqtSlot(result=str)
    def getSystemInfo(self):
        try:
            result = self.systemInfoIface.getSystemInfo()
        except Exception, e:
            print e
            return ""
        return str(result)
