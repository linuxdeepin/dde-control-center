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
from PyQt5.QtQuick import QQuickWindow
from PyQt5.QtCore import pyqtSlot, Qt
from PyQt5.QtGui import QSurfaceFormat, QColor
from PyQt5 import QtCore, QtWidgets, QtGui

class PyQtWindow(QQuickWindow):

    mousePressed = QtCore.pyqtSignal(QtCore.QPointF)
    focusLosed =QtCore.pyqtSignal()

    def __init__(self, test):
        QQuickWindow.__init__(self)
        surface_format = QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)
        self.setFormat(surface_format)

    def onFocusWindowChanged(self, win):    
        if  win.__class__.__name__   != "QWindow":
            self._calendar.hide()
        if win is None:    
            self.focusLosed.emit()

    def mousePressEvent(self, event):    
        self.mousePressed.emit(QtCore.QPointF(event.x(), event.y()))
        return super(PyQtWindow, self).mousePressEvent(event)
