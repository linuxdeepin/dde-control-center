#! /usr/bin/python
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
import sys
from datetime import datetime

from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5 import QtDBus
from PyQt5 import QtWidgets
from PyQt5 import QtQuick

from display_monitor import connect_to_primary_changed
from constants import ROOT_LOCATION, PANEL_WIDTH
from constants import APP_DBUS_NAME
from modules_info import ModulesId
from nls import QtGettext
from ChineseLunar import ChineseCalendar150
from dialog_window import MessageDialog
import utils
import ip_utils

class DssDbusAdptor(QtDBus.QDBusAbstractAdaptor):
    QtCore.Q_CLASSINFO("D-Bus Interface", APP_DBUS_NAME)

    def __init__(self, parent):
        QtDBus.QDBusAbstractAdaptor.__init__(self, parent)
        self.setAutoRelaySignals(True)

    @QtCore.pyqtSlot(int)
    def Show(self, seconds):
        self.parent().show(seconds)

    @QtCore.pyqtSlot(str)
    def ShowModule(self, moduleName):
        self.parent().view_object.showModule(moduleName)

    @QtCore.pyqtSlot()
    def Hide(self):
        self.parent().view_object.hideDss()
    
    @QtCore.pyqtSlot(int, int)
    def ClickToHide(self, mouseX, mouseY):
        self.parent().view_object.outerAreaClicked(mouseX, mouseY)

    @QtCore.pyqtSlot()
    def Toggle(self):
        self.parent().view_object.togglePanel()

class ControlPanel(QtQuick.QQuickView):

    moduleFileChanged = QtCore.pyqtSignal(str)
    focusLosed = QtCore.pyqtSignal()

    def __init__(self):
        QtQuick.QQuickView.__init__(self)

        surface_format = QtGui.QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)

        self.setColor(QtGui.QColor(0, 0, 0, 0))
        self.setFlags(
                QtCore.Qt.Tool
                | QtCore.Qt.FramelessWindowHint
                | QtCore.Qt.WindowStaysOnTopHint
                | QtCore.Qt.X11BypassWindowManagerHint
                )
        self.setResizeMode(QtQuick.QQuickView.SizeRootObjectToView)
        self.setFormat(surface_format)
        self.set_all_contexts()
        self.setSource(QtCore.QUrl.fromLocalFile(os.path.join(
            ROOT_LOCATION, 'frame', 'views', 'Main.qml')))
        self.connect_all_object_function()

        self.engine_obj = self.engine()
        connect_to_primary_changed(self.display_primary_changed)

        self._dbus_adptor = DssDbusAdptor(self)
        QtWidgets.qApp.focusWindowChanged.connect(self.onFocusWindowChanged)

    def onFocusWindowChanged(self, win):
        if win is None:
            #print "focusLosed"
            self.focusLosed.emit()
        else:
            pass
            #print "Focus Window:", win.__class__.__name__

    def set_all_contexts(self):
        self.qml_context = self.rootContext()
        self.modulesId = ModulesId()
        self.qtGettext = QtGettext()
        self.message_view = MessageDialog()
        self.qml_context.setContextProperty("windowView", self)
        self.qml_context.setContextProperty("modulesId", self.modulesId)
        self.qml_context.setContextProperty("messageView", self.message_view)

    def connect_all_object_function(self):
        self.view_object = self.rootObject()

    def set_geometry(self, rect):
        x, y, width, height = rect
        self.setGeometry(x + width, y,
                PANEL_WIDTH, height)

    @QtCore.pyqtSlot(str)
    def setCustomCursor(self, path):
        pixmap = QtGui.QPixmap(path.split("//")[1])
        cursor = QtGui.QCursor(pixmap, -1, -1)
        QtGui.QGuiApplication.restoreOverrideCursor()
        QtGui.QGuiApplication.setOverrideCursor(cursor)
        QtGui.QGuiApplication.changeOverrideCursor(cursor)

    @QtCore.pyqtSlot()
    def clearCustomCursor(self):
        QtGui.QGuiApplication.restoreOverrideCursor()

    @QtCore.pyqtSlot(int)
    def setCursorFlashTime(self, time):
        QtWidgets.QApplication.setCursorFlashTime(time)

    @QtCore.pyqtSlot(str, result=str)
    def getDefaultMask(self, ip_addr):
        return ip_utils.getDefaultMask(ip_addr)

    @QtCore.pyqtSlot(result=int)
    def getPid(self):
        return os.getpid()
        
    @QtCore.pyqtSlot(result=str)
    def getHomeDir(self):
        return os.path.expanduser("~")

    @QtCore.pyqtSlot(int)
    def show(self, seconds):
        self.view_object.showDss(seconds)

    @QtCore.pyqtProperty(int)
    def panelWith(self):
        return PANEL_WIDTH

    @QtCore.pyqtSlot(QtDBus.QDBusMessage)
    def display_primary_changed(self, message):
        rect = QtDBus.QDBusReply(message).value()
        self.set_geometry(rect)

    @QtCore.pyqtSlot(result=QtCore.QVariant)
    def getCursorPos(self):
        qpoint = self.cursor().pos()
        return [qpoint.x(), qpoint.y()]

    @QtCore.pyqtSlot(str, result=str)
    def stripString(self, s):
        return s.strip()

    @QtCore.pyqtSlot(QtCore.QVariant, str, result=str)
    def joinString(self, s_list, key):
        return key.join(s_list)

    @QtCore.pyqtSlot(str, result=bool)
    def isIconPluginExist(self, module_id):
        return os.path.exists(os.path.join(ROOT_LOCATION, "modules", module_id, "iconPlugin.qml"))

    @QtCore.pyqtSlot(str, result=str)
    def toHumanShortcutLabel(self, sequence):
        sequence = sequence.replace("<", "").replace(">", "+")
        keys = sequence.split("-")
        return "+".join(keys).title()

    @QtCore.pyqtSlot(str, result=str)
    def toHumanThemeName(self, sequence):
        keys = sequence.split("-")
        return " ".join(keys).title()

    @QtCore.pyqtSlot(result=QtCore.QVariant)
    def argv(self):
        return sys.argv[1:]

    @QtCore.pyqtSlot(QtCore.QVariant, bool, result=QtCore.QVariant)
    def sortArray(self, data, reverse=False):
        utils.quicksort(data)
        if reverse:
            data.reverse()
        return data

    @QtCore.pyqtSlot(str, result=QtCore.QVariant)
    def getLunarDay(self, value):
        dt = datetime.strptime(value, "%Y-%m-%d")
        cc150 = ChineseCalendar150(dt)
        return [cc150.get_lunar_day(), cc150.GetFullGanzhiDate1(), cc150.GetJieQi()]
