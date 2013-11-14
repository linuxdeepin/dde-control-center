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
from PyQt5.QtDBus import QDBusAbstractInterface, QDBusConnection, QDBusReply

class SystemInfoInterface(QDBusAbstractInterface):
    
    def __init__(self):
        super(SystemInfoInterface, self).__init__("com.deepin.daemon.SystemInfo", # service_name
                                                  "/com/deepin/daemon/SystemInfo", # path
                                                  'org.freedesktop.DBus.Properties', # interface
                                                  QDBusConnection.sessionBus(), None)
        
    def getPropertyValue(self, interfaceName, propertyName):
        msg = self.call("Get", interfaceName, propertyName)
        reply = QDBusReply(msg) 
        return reply.value()
        
    def getVersionInfo(self):
        return self.getPropertyValue("com.deepin.daemon.SystemInfo", "Version")
    
    def getDiskInfo(self):
        return self.getPropertyValue("com.deepin.daemon.SystemInfo", "DiskCap")
    
    def getMemoryInfo(self):
        return self.getPropertyValue("com.deepin.daemon.SystemInfo", "MemorySize")
    
    def getCpuInfo(self):
        return self.getPropertyValue("com.deepin.daemon.SystemInfo", "Processor")
    
    def getArchitectureInfo(self):
        return self.getPropertyValue("com.deepin.daemon.SystemInfo", "SystemType")
    
class IfaceWrapper(QObject):
    def __init__(self, systemInfoIface):
        self.systemInfoIface = systemInfoIface
        
    @pyqtSlot(result=str)
    def getVersionInfo(self):
        return str(self.systemInfoIface.getVersionInfo())
    
    @pyqtSlot(result=str)    
    def getDiskInfo(self):
        return str(self.systemInfoIface.getDiskInfo())
    
    @pyqtSlot(result=str)    
    def getMemoryInfo(self):
        return str(self.systemInfoIface.getMemoryInfo())
    
    @pyqtSlot(result=str)    
    def getCpuInfo(self):
        return str(self.systemInfoIface.getCpuInfo())
    
    @pyqtSlot(result=str)    
    def getArchitectureInfo(self):
        return str(self.systemInfoIface.getArchitectureInfo())

if __name__ == "__main__":
    from PyQt5.QtCore import QCoreApplication
    
    app = QCoreApplication([])
    
    wrapper = IfaceWrapper(SystemInfoInterface())
    print wrapper.getVersionInfo()
    print wrapper.getArchitectureInfo()
    print wrapper.getCpuInfo()
    print wrapper.getDiskInfo()
    print wrapper.getMemoryInfo()
    
    app.exec_()
