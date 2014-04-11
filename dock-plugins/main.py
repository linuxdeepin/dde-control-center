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

import os
from PyQt5 import QtCore
if os.name == 'posix':
    QtCore.QCoreApplication.setAttribute(QtCore.Qt.AA_X11InitThreads, True)

import sys
from PyQt5 import QtGui
from PyQt5 import QtQml
from PyQt5 import QtQuick
app = QtGui.QGuiApplication(sys.argv)

import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

class MainObject(QtCore.QObject):
    def __init__(self, qml_path):
        QtCore.QObject.__init__(self)

        self.engine = QtQml.QQmlEngine()

        self.rootContext = QtQml.QQmlContext(self.engine, self)
        self.set_all_contexts()

        self.component = QtQml.QQmlComponent(self.engine, self)
        QtQuick.QQuickWindow.setDefaultAlphaBuffer(True)

        self.component.loadUrl(QtCore.QUrl.fromLocalFile(qml_path))

        self.component.beginCreate(self.rootContext)
        if self.component.isReady():
            self.rootObject = self.component.completeCreate()
        else:
            print self.component.errorString()
            sys.exit(1)

        sys.exit(app.exec_())

    def set_all_contexts(self):
        print self.rootContext
        self.rootContext.setContextProperty("mainObject", self)

    @QtCore.pyqtSlot(result=int)
    def getPid(self):
        return os.getpid()
    
if __name__ == "__main__":
    MainObject("qml/main.qml")

