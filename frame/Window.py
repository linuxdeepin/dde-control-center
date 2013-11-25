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

from PyQt5.QtQuick import QQuickView
from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtGui import QSurfaceFormat, QColor
import subprocess

class Window(QQuickView):

    def __init__(self):
        QQuickView.__init__(self)
        surface_format = QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)
        
        self.setColor(QColor(0, 0, 0, 0))
        self.setFlags(
                Qt.FramelessWindowHint
                | Qt.WindowStaysOnTopHint
                | Qt.X11BypassWindowManagerHint
                | Qt.Popup
                )
        self.setResizeMode(QQuickView.SizeRootObjectToView)
        self.setFormat(surface_format)

    @pyqtSlot()
    def shutdown(self):
        subprocess.Popen(['/data/share/Projects/deepin/dde/build/shutdown'])
