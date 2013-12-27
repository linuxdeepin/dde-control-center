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

from PyQt5.QtCore import Qt, pyqtSlot, pyqtSignal, QVariant, QUrl, QFileSystemWatcher
from PyQt5.QtGui import QSurfaceFormat, QColor, QKeySequence
from PyQt5.QtQuick import QQuickView

from constants import SHUT_DOWN_ORDER_PATH, ROOT_LOCATION
from modules_info import ModulesId
from event import RecordEvent
from nls import QtGettext

def walk_directory(root_dir):
    for (root, folder, files) in os.walk(root_dir):
        for f in files:
            path = os.path.join(root, f)
            yield root, path

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
        self.screen_size = self.screen().size()
        self.setGeometry(self.screen_size.width(), 0,
                360, self.screen_size.height())
        self.set_all_contexts()
        self.setSource(QUrl.fromLocalFile(os.path.join(
            ROOT_LOCATION, 'frame', 'views', 'Main.qml')))
        self.connect_all_object_function()

        self.engine_obj = self.engine()

        self.modules_dir = os.path.join(ROOT_LOCATION, 'modules')
        self.module_file_monotor = QFileSystemWatcher()
        for root, path in walk_directory(self.modules_dir):
            self.module_file_monotor.addPath(path)
            self.module_file_monotor.addPath(root)

        self.module_file_monotor.fileChanged.connect(self.fileChangedNotify)
        #self.module_file_monotor.directoryChanged.connect(self.fileChangedNotify)

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

    def set_all_contexts(self):
        self.qml_context = self.rootContext()
        self.modulesId = ModulesId()
        self.qtGettext = QtGettext()
        self.qml_context.setContextProperty("windowView", self)
        self.qml_context.setContextProperty("screenSize", self.screen_size)
        self.qml_context.setContextProperty("modulesId", self.modulesId)
        self.qml_context.setContextProperty("qtgettext", self.qtGettext)

    def connect_all_object_function(self):
        self.view_object = self.rootObject()
        self.record_event = RecordEvent(self)
        self.record_event.enter_mouse_area.connect(self.view_object.displayTrayIcon)
        self.record_event.click_outer_area.connect(self.view_object.outerAreaClicked)
        self.moduleFileChanged.connect(self.view_object.moduleFileChanged)

    @pyqtSlot(str, result=str)
    def toHumanShortcutLabel(self, sequence):
        keys = sequence.split("-")
        return "+".join(keys).title()

    @pyqtSlot(bool, result=bool)
    def grabKeyboard(self, flag):
        return self.setKeyboardGrabEnabled(flag)

    @pyqtSlot()
    def shutdown(self):
        if os.path.exists(SHUT_DOWN_ORDER_PATH):
            subprocess.Popen([SHUT_DOWN_ORDER_PATH])
        else:
            print "File not exist:", SHUT_DOWN_ORDER_PATH

    @pyqtSlot(result=QVariant)
    def argv(self):
        return sys.argv[1:]
