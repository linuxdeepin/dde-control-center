#! /usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2011 ~ 2012 Deepin, Inc.
#               2011 ~ 2012 Wang Yong
# 
# Author:     Wang Yong <lazycat.manatee@gmail.com>
# Maintainer: Wang Yong <lazycat.manatee@gmail.com>
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

from PyQt5.QtCore import pyqtSlot, QObject, pyqtSignal
from PyQt5.QtDBus import QDBusConnection, QDBusInterface
import sys

class UniqueService(QObject):
    uniqueTrigger = pyqtSignal()    
    
    def __init__(self, dbus_name, object_name):
        QObject.__init__(self)
        
        iface = QDBusInterface(dbus_name, object_name, '', QDBusConnection.sessionBus())
        if iface.isValid():
            iface.call("unique")
            sys.exit(1)
        
        QDBusConnection.sessionBus().registerService(dbus_name)
        QDBusConnection.sessionBus().registerObject(object_name, self, QDBusConnection.ExportAllSlots)
    
    @pyqtSlot()
    def unique(self):
        self.uniqueTrigger.emit()
        
