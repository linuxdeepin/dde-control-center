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

DDE_DOCK_APPLET_MANAGER_NAME = "dde.dock.entry.AppletManager"
DDE_DOCK_APPLET_MANAGER_PATH = "/dde/dock/entry/AppletManager"

ROOT_DIR = os.path.dirname(os.path.realpath(__file__))

def registerDBus(dbus_name, path, obj):
    session_bus = QtDBus.QDBusConnection.sessionBus()
    session_bus.registerService(dbus_name)
    session_bus.registerObject(path, obj)

class DockAppletManagerAdptor(QtDBus.QDBusAbstractAdaptor):
    QtCore.Q_CLASSINFO("D-Bus Interface", DDE_DOCK_APPLET_MANAGER_NAME)
    QtCore.Q_CLASSINFO("D-Bus Introspection",
                '  <interface name="%s">\n'
                '    <method name="ShowApplet">\n'
                '      <arg direction="in" type="s" name="name"/>\n'
                '    </method>\n'
                '    <method name="HideApplet">\n'
                '      <arg direction="in" type="s" name="name"/>\n'
                '    </method>\n'
                '    <property name="AppletList" type="as" access="read"/>\n'
                '  </interface>\n' % DDE_DOCK_APPLET_MANAGER_NAME)

    _AppletListChanged = QtCore.pyqtSignal()

    def __init__(self, parent):
        QtDBus.QDBusAbstractAdaptor.__init__(self, parent)
        self.setAutoRelaySignals(True)

    @QtCore.pyqtSlot(str)
    def HideApplet(self, name):
        self.parent().rootObject.set_hide_applet(name)

    @QtCore.pyqtSlot(str)
    def ShowApplet(self, name):
        self.parent().rootObject.set_show_applet(name)

    @QtCore.pyqtProperty("QVariant", notify=_AppletListChanged)
    def AppletList(self):
        return self.parent().get_applet_list()

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

    @QtCore.pyqtSlot(result="QVariant")
    def get_applet_list(self):
        applets = {}
        for name in os.listdir(os.path.join(ROOT_DIR, "qml")):
            appet_path = os.path.join(ROOT_DIR, "qml", name)
            if os.path.isdir(appet_path):
                main_qml = os.path.join(appet_path, "main.qml")
                if os.path.exists(main_qml):
                    applets[name] = main_qml
        return applets.keys()

    @QtCore.pyqtSlot(result=int)
    def getPid(self):
        return os.getpid()
    
if __name__ == "__main__":
    qml_path = os.path.join(ROOT_DIR, "qml/main.qml")
    t = MainObject(qml_path)
    registerDBus(DDE_DOCK_APPLET_MANAGER_NAME, DDE_DOCK_APPLET_MANAGER_PATH, t)
    sys.exit(app.exec_())
