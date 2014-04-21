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
from PyQt5 import QtDBus
app = QtGui.QGuiApplication(sys.argv)

import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

DDE_DOCK_APPLET_MANAGER_NAME = "com.deepin.dde.dock.applet.manager"
DDE_DOCK_APPLET_MANAGER_PATH = "/com/deepin/dde/dock/applet/manager"

def registerDBus(dbus_name, path, obj):
    session_bus = QtDBus.QDBusConnection.sessionBus()
    session_bus.registerService(dbus_name)
    session_bus.registerObject(path, obj)

class DockAppletManagerAdptor(QtDBus.QDBusAbstractAdaptor):
    QtCore.Q_CLASSINFO("D-Bus Interface", DDE_DOCK_APPLET_MANAGER_NAME)

    AllAppletsChanged = QtCore.pyqtSignal()

    def __init__(self, parent):
        QtDBus.QDBusAbstractAdaptor.__init__(self, parent)
        self.setAutoRelaySignals(True)

    @QtCore.pyqtSlot(str)
    def SetHideApplet(self, name):
        self.parent().rootObject.set_hide_applet(name)

    @QtCore.pyqtSlot(str)
    def SetShowApplet(self, name):
        self.parent().rootObject.set_show_applet(name)

class MainObject(QtCore.QObject):
    def __init__(self, qml_path):
        QtCore.QObject.__init__(self)

        self.engine = QtQml.QQmlEngine()

        self.rootContext = QtQml.QQmlContext(self.engine, self)
        self.set_all_contexts()

        self.component = QtQml.QQmlComponent(self.engine, self)
        QtQuick.QQuickWindow.setDefaultAlphaBuffer(True)

        self.component.loadUrl(QtCore.QUrl.fromLocalFile(qml_path))

        self.rootObject = self.component.beginCreate(self.rootContext)
        if self.component.isReady():
            self.component.completeCreate()
        else:
            print self.component.errorString()
            sys.exit(1)

        self._adaptor = DockAppletManagerAdptor(self)

    def set_all_contexts(self):
        self.rootContext.setContextProperty("mainObject", self)

    @QtCore.pyqtSlot(result=int)
    def getPid(self):
        return os.getpid()
    
if __name__ == "__main__":
    qml_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "qml/main.qml")
    t = MainObject(qml_path)
    registerDBus(DDE_DOCK_APPLET_MANAGER_NAME, DDE_DOCK_APPLET_MANAGER_PATH, t)
    sys.exit(app.exec_())
