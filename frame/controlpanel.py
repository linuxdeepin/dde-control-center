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
import subprocess
from threading import Timer
from datetime import datetime

from PyQt5.QtCore import (Qt, pyqtSlot, pyqtSignal, QVariant, QUrl,
        QFileSystemWatcher, pyqtProperty, Q_CLASSINFO)
from PyQt5.QtGui import QSurfaceFormat, QColor
from PyQt5.QtQuick import QQuickView
from PyQt5.QtDBus import QDBusMessage, QDBusReply, QDBusAbstractAdaptor

from display_monitor import RecordEvent
from constants import ROOT_LOCATION, PANEL_WIDTH
from constants import APP_DBUS_NAME, APP_OBJECT_PATH
from modules_info import ModulesId
from nls import QtGettext
from ChineseLunar import ChineseCalendar150

def walk_directory(root_dir):
    for (root, folder, files) in os.walk(root_dir):
        for f in files:
            path = os.path.join(root, f)
            yield root, path

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
            '    <method name="show"/>\n'
            '  </interface>\n'
            """ % APP_DBUS_NAME)

    def __init__(self, parent):
        super(DBusService, self).__init__(parent)

        self.setAutoRelaySignals(True)

    @pyqtSlot()
    def show(self):
        self.parent().show()


class ControlPanel(QQuickView):

    moduleFileChanged = pyqtSignal(str)

    def __init__(self):
        QQuickView.__init__(self)

        surface_format = QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)
        
        self.timer = None
        self.setColor(QColor(0, 0, 0, 0))
        self.setFlags(
                Qt.FramelessWindowHint
                | Qt.WindowStaysOnTopHint
                | Qt.X11BypassWindowManagerHint
                | Qt.Popup
                )
        self.setResizeMode(QQuickView.SizeRootObjectToView)
        self.setFormat(surface_format)
        self.record_event = RecordEvent(self)
        self.set_all_contexts()
        self.setSource(QUrl.fromLocalFile(os.path.join(
            ROOT_LOCATION, 'frame', 'views', 'Main.qml')))
        self.connect_all_object_function()

        self.engine_obj = self.engine()

        ### file monitor
        self.modules_dir = os.path.join(ROOT_LOCATION, 'modules')
        self.module_file_monotor = QFileSystemWatcher()
        for root, path in walk_directory(self.modules_dir):
            self.module_file_monotor.addPath(path)
            self.module_file_monotor.addPath(root)

        self.module_file_monotor.fileChanged.connect(self.fileChangedNotify)
        #self.module_file_monotor.directoryChanged.connect(self.fileChangedNotify)
        ### file monitor

    def set_all_contexts(self):
        self.qml_context = self.rootContext()
        self.modulesId = ModulesId()
        self.qtGettext = QtGettext()
        self.qml_context.setContextProperty("windowView", self)
        self.qml_context.setContextProperty("modulesId", self.modulesId)
        #self.qml_context.setContextProperty("qtgettext", self.qtGettext)

    def connect_all_object_function(self):
        self.view_object = self.rootObject()
        self.record_event.enter_mouse_area.connect(self.view_object.displayTrayIcon)
        self.record_event.click_outer_area.connect(self.view_object.outerAreaClicked)
        #self.moduleFileChanged.connect(self.view_object.moduleFileChanged)

    @pyqtSlot()
    def show(self):
        self.view_object.displayTrayIcon()

    def set_geometry(self, rect):
        x, y, width, height = rect
        self.setGeometry(x + width, y,
                PANEL_WIDTH, height)

    def fileChangedNotify(self, path):
        self.engine_obj.clearComponentCache()
        module_id = path.split(self.modules_dir)[1].split("/")[1]
        module_dir = os.path.join(ROOT_LOCATION, 'modules', module_id)
        for r, p in walk_directory(module_dir):
            if p not in self.module_file_monotor.files():
                self.module_file_monotor.addPath(p)

        if self.timer:
            self.timer.cancel()
        self.timer = Timer(0.2, lambda : self.moduleFileChanged.emit(module_id))
        self.timer.start()

    @pyqtProperty(int)
    def panelWith(self):
        return PANEL_WIDTH

    @pyqtSlot(str, result=str)
    def getModuleDataDir(self, module_id):
        return os.path.join(ROOT_LOCATION, "data", module_id)

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
