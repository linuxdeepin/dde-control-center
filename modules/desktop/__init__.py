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

from PyQt5.QtCore import QObject, pyqtSlot, pyqtSignal
from PyQt5.QtDBus import QDBusAbstractInterface, QDBusConnection, QDBusReply

class DesktopSettingsInterface(QDBusAbstractInterface):
    PropertyChanged = pyqtSignal()
    
    def __init__(self):
        super(DesktopSettingsInterface, self).__init__("com.deepin.daemon.Desktop", # service_name
                                                       "/com/deepin/daemon/Desktop", # path
                                                       'org.freedesktop.DBus.Properties', # interface
                                                       QDBusConnection.sessionBus(), None)
        
    def getPropertyValue(self, interfaceName, propertyName):
        msg = self.call("Get", interfaceName, propertyName)
        reply = QDBusReply(msg) 
        return reply.value()
        
    def setPropertyValue(self, interfaceName, propertyName):
        self.call("Set", interfaceName, propertyName)
        
    def getShowComputerIcon(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "ShowComputerIcon")
    
    def getShowDSCIcon(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "ShowDSCIcon")
    
    def getShowHomeIcon(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "ShowHomeIcon")
    
    def getShowTrashIcon(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "ShowTrashIcon")
    
    def getTopLeftAction(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "TopLeftAction")
    
    def getBottomRightAction(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "BottomRightAction")
    
    def getDockMode(self):
        return self.getPropertyValue("com.deepin.daemon.Desktop", "DockMode")
    
    
    def setShowComputerIcon(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "ShowComputerIcon", value)
    
    def setShowDSCIcon(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "ShowDSCIcon", value)
    
    def setShowHomeIcon(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "ShowHomeIcon", value)
    
    def setShowTrashIcon(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "ShowTrashIcon", value)
    
    def setTopLeftAction(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "TopLeftAction", value)
    
    def setBottomRightAction(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "BottomRightAction", value)
    
    def setDockMode(self, value):
        self.setPropertyValue("com.deepin.daemon.Desktop", "DockMode", value)
    
class IfaceWrapper(QObject):
    def __init__(self, desktopSettingsInterface):
        self.desktopSettingsInterface = desktopSettingsInterface
        
    @pyqtSlot(result=bool)
    def getShowComputerIcon(self):
        return self.desktopSettingsInterface.getShowComputerIcon()
    
    @pyqtSlot(result=bool)    
    def getShowDSCIcon(self):
        return self.desktopSettingsInterface.getShowDSCIcon()
    
    @pyqtSlot(result=bool)    
    def getShowHomeIcon(self):
        return self.desktopSettingsInterface.getShowHomeIcon()
    
    @pyqtSlot(result=bool)    
    def getShowTrashIcon(self):
        return self.desktopSettingsInterface.getShowTrashIcon()
    
    @pyqtSlot(result=int)    
    def getTopLeftAction(self):
        return self.desktopSettingsInterface.getTopLeftAction()    
    
    @pyqtSlot(result=int)    
    def getBottomRightAction(self):
        return self.desktopSettingsInterface.getBottomRightAction()    
    
    @pyqtSlot(result=int)    
    def getDockMode(self):
        return self.desktopSettingsInterface.getDockMode()    

    @pyqtSlot(bool)
    def setShowComputerIcon(self, value):
        self.desktopSettingsInterface.setShowComputerIcon(value)
    
    @pyqtSlot(bool)
    def setShowDSCIcon(self, value):
        self.desktopSettingsInterface.setShowDSCIcon(value)
    
    @pyqtSlot(bool)    
    def setShowHomeIcon(self, value):
        self.desktopSettingsInterface.setShowHomeIcon(value)
    
    @pyqtSlot(bool)    
    def setShowTrashIcon(self, value):
        self.desktopSettingsInterface.setShowTrashIcon(value)
    
    @pyqtSlot(int)
    def setTopLeftAction(self, value):
        self.desktopSettingsInterface.setTopLeftAction(value)    
    
    @pyqtSlot(int)     
    def setBottomRightAction(self, value):
        self.desktopSettingsInterface.setBottomRightAction(value)    
    
    @pyqtSlot(int)             
    def setDockMode(self, value):
        self.desktopSettingsInterface.setDockMode(value)    

if __name__ == "__main__":
    from PyQt5.QtCore import QCoreApplication
    
    app = QCoreApplication([])
    
    wrapper = IfaceWrapper(DesktopSettingsInterface())
    
    app.exec_()
