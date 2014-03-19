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

from PyQt5.QtCore import (Qt, pyqtSlot, pyqtSignal, QVariant, QUrl,
        pyqtProperty, Q_CLASSINFO)
from PyQt5.QtGui import QSurfaceFormat, QColor
from PyQt5.QtQuick import QQuickView
from PyQt5.QtDBus import QDBusMessage, QDBusReply, QDBusAbstractAdaptor

from display_monitor import RecordEvent, connect_to_primary_changed
from constants import ROOT_LOCATION, PANEL_WIDTH
from constants import APP_DBUS_NAME
from modules_info import ModulesId
from nls import QtGettext
from ChineseLunar import ChineseCalendar150
from dialog_window import MessageDialog

def quicksort(data, low = 0, high = None):
    if high == None:
        high = len(data) - 1
    if low < high:
        s, i, j = data[low], low, high
        while i < j:
            while i < j and data[j] >= s:
                j = j - 1
            if i < j:
                data[i] = data[j]
                i = i + 1
            while i < j and data[i] <= s:
                i = i + 1
            if i < j:
                data[j] = data[i]
                j = j - 1
        data[i] = s
        quicksort(data, low, i - 1)
        quicksort(data, i + 1, high)

class DBusService(QDBusAbstractAdaptor):
    Q_CLASSINFO("D-Bus Interface", APP_DBUS_NAME)

    Q_CLASSINFO("D-Bus Introspection", """
            '  <interface name="%s">\n'
            '    <method name="show">\n'
            '      <arg direction="in" type="i" name="seconds"/>\n'
            '    </method>
            '    <method name="showModule">\n'
            '      <arg direction="in" type="s" name="moduleName"/>\n'
            '    </method>
            '  </interface>\n'
            """ % APP_DBUS_NAME)

    def __init__(self, parent):
        QDBusAbstractAdaptor.__init__(self, parent)
        self.setAutoRelaySignals(True)

    @pyqtSlot(int)
    def show(self, seconds):
        self.parent().show(seconds)

    @pyqtSlot(str)
    def showModule(self, moduleName):
        self.parent().view_object.showModule(moduleName)

class ControlPanel(QQuickView):

    moduleFileChanged = pyqtSignal(str)

    def __init__(self):
        QQuickView.__init__(self)

        surface_format = QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)
        
        self.setColor(QColor(0, 0, 0, 0))
        self.setFlags(
                Qt.FramelessWindowHint
                | Qt.WindowStaysOnTopHint
                )
        self.setResizeMode(QQuickView.SizeRootObjectToView)
        self.setFormat(surface_format)
        self.record_event = RecordEvent(self)
        self.set_all_contexts()
        self.setSource(QUrl.fromLocalFile(os.path.join(
            ROOT_LOCATION, 'frame', 'views', 'Main.qml')))
        self.connect_all_object_function()

        self.engine_obj = self.engine()

        connect_to_primary_changed(self.display_primary_changed)

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
        self.record_event.enter_mouse_area.connect(lambda :self.view_object.showDss(0))
        self.record_event.click_outer_area.connect(self.view_object.outerAreaClicked)

    def set_geometry(self, rect):
        x, y, width, height = rect
        self.setGeometry(x + width, y,
                PANEL_WIDTH, height)

    @pyqtSlot(int)
    def show(self, seconds):
        self.view_object.showDss(seconds)

    @pyqtProperty(int)
    def panelWith(self):
        return PANEL_WIDTH

    @pyqtSlot(result=bool)
    def getDisplayConfigExists(self):
        return os.path.exists(os.path.expanduser("~/.config/deepin_monitors.json"))

    @pyqtSlot(QDBusMessage)
    def display_primary_changed(self, message):
        rect = QDBusReply(message).value()
        self.set_geometry(rect)

    @pyqtSlot(result=QVariant)
    def getCursorPos(self):
        qpoint = self.cursor().pos()
        return [qpoint.x(), qpoint.y()]

    @pyqtSlot(str, result=str)
    def stripString(self, s):
        return s.strip()

    @pyqtSlot(str, result=bool)
    def isIconPluginExist(self, module_id):
        return os.path.exists(os.path.join(ROOT_LOCATION, "modules", module_id, "iconPlugin.qml"))

    @pyqtSlot(str, result=str)
    def toHumanShortcutLabel(self, sequence):
        sequence = sequence.replace("<", "").replace(">", "+")
        keys = sequence.split("-")
        return "+".join(keys).title()

    @pyqtSlot(str, result=str)
    def toHumanThemeName(self, sequence):
        keys = sequence.split("-")
        return " ".join(keys).title()

    @pyqtSlot(result=QVariant)
    def argv(self):
        return sys.argv[1:]

    @pyqtSlot(QVariant, bool, result=QVariant)
    def sortArray(self, data, reverse=False):
        quicksort(data)
        if reverse:
            data.reverse()
        return data

    @pyqtSlot(str, result=QVariant)
    def getLunarDay(self, value):
        dt = datetime.strptime(value, "%Y-%m-%d")
        cc150 = ChineseCalendar150(dt)
        return [cc150.get_lunar_day(), cc150.GetFullGanzhiDate1(), cc150.GetJieQi()]
