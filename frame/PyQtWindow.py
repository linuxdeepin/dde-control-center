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

from PyQt5.QtCore import QSize
from PyQt5 import QtGui
from PyQt5.QtQuick import QQuickView
from PyQt5.QtCore import pyqtSlot, Qt
from PyQt5.QtGui import QSurfaceFormat, QColor

class PyQtWindow(QQuickView):

    def __init__(self, test):
        QQuickView.__init__(self)
        surface_format = QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)
        
        self.setColor(QColor(0, 0, 0, 0))
        self.setFlags(
                Qt.FramelessWindowHint
                )
        self.setResizeMode(QQuickView.SizeRootObjectToView)
        self.setFormat(surface_format)
        self.qml_context = self.rootContext()
        print test
        
    @pyqtSlot(result=int)    
    def getState(self):
        return self.windowState()
    
    @pyqtSlot()
    def doMinimized(self):
        # NOTE: This is bug of Qt5 that showMinimized() just can work once after restore window.
        # I change window state before set it as WindowMinimized to fixed this bug!
        self.setWindowState(Qt.WindowMaximized)
        
        # Do minimized.
        self.setWindowState(Qt.WindowMinimized)
        self.setVisible(True)
        
    @pyqtSlot(result="QVariant")    
    def getCursorPos(self):
        return QtGui.QCursor.pos()        
    
    @pyqtSlot(int, int)
    def setMinSize(self, min_width, min_height):
        self.setMinimumSize(QSize(min_width, min_height))

