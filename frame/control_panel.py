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

from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5 import QtDBus
from PyQt5 import QtWidgets
from PyQt5 import QtQuick
from PyQt5 import QtQml

from constants import ROOT_LOCATION, PANEL_WIDTH
from constants import APP_DBUS_NAME
import utils
import ip_utils

class DssDbusAdptor(QtDBus.QDBusAbstractAdaptor):
    QtCore.Q_CLASSINFO("D-Bus Interface", APP_DBUS_NAME)

    def __init__(self, parent):
        QtDBus.QDBusAbstractAdaptor.__init__(self, parent)
        self.setAutoRelaySignals(True)

    @QtCore.pyqtSlot()
    def Toggle(self):
        self.parent().view_object.togglePanel()

    @QtCore.pyqtSlot()
    def Show(self):
        self.parent().view_object.showDss(0)

    @QtCore.pyqtSlot(str)
    def ShowModule(self, moduleName):
        self.parent().view_object.showModule(moduleName)

    @QtCore.pyqtSlot(result=bool)
    def isNetworkCanShowPassword(self):
        return self.parent().view_object.getNetworkCanShowPassword()

    @QtCore.pyqtSlot()
    def ShowImmediately(self):
        self.parent().view_object.showDssImmediately()

    @QtCore.pyqtSlot()
    def Hide(self):
        self.parent().view_object.hideDss()

    @QtCore.pyqtSlot()
    def HideImmediately(self):
        self.parent().view_object.hideDssImmediately()

class ControlPanel(QtCore.QObject):

    moduleFileChanged = QtCore.pyqtSignal(str)
    focusLosed = QtCore.pyqtSignal()

    def __init__(self):
        QtCore.QObject.__init__(self)

        self.engine = QtQml.QQmlEngine()

        # set context
        self.rootContext = QtQml.QQmlContext(self.engine, self)
        self.set_all_contexts()

        self.component = QtQml.QQmlComponent(self.engine, self)
        QtQuick.QQuickWindow.setDefaultAlphaBuffer(True)

        self.component.loadUrl(QtCore.QUrl.fromLocalFile(
            os.path.join(ROOT_LOCATION, 'frame', 'views', 'Main.qml')))

        self.view_object = self.component.beginCreate(self.rootContext)
        if self.component.isReady():
            self.component.completeCreate()
        else:
            print self.component.errorString()
            sys.exit(1)

        self._dbus_adptor = DssDbusAdptor(self)
        QtWidgets.qApp.focusWindowChanged.connect(self.onFocusWindowChanged)

    def onFocusWindowChanged(self, win):
        if win is None:
            self.focusLosed.emit()
        else:
            pass

    def set_all_contexts(self):
        self.rootContext.setContextProperty("windowView", self)

    @QtCore.pyqtSlot(str)
    def installPackage(self, pkg_name):
        import software_center
        software_center.install(pkg_name)

    @QtCore.pyqtSlot(str, result=str)
    def getGplText(self, lang):
        path = os.path.join(ROOT_LOCATION, "frame", "gpl", "gpl-3.0-%s.txt" % lang)
        if not os.path.exists(path):
            path = os.path.join(ROOT_LOCATION, "frame", "gpl", "gpl-3.0.txt")

        with open(path) as fp:
            return fp.read()

    @QtCore.pyqtSlot(str, result=str)
    def getGplTextTitle(self, lang):
        path = os.path.join(ROOT_LOCATION, "frame", "gpl", "gpl-3.0-%s-title.txt" % lang)
        if not os.path.exists(path):
            path = os.path.join(ROOT_LOCATION, "frame", "gpl", "gpl-3.0-title.txt")

        with open(path) as fp:
            return fp.read()

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

    @QtCore.pyqtSlot(result=str)
    def getHomeDir(self):
        return os.path.expanduser("~")

    @QtCore.pyqtSlot(str, result=str)
    def toHumanShortcutLabel(self, sequence):
        sequence = sequence.replace("<", "").replace(">", "+")
        keys = sequence.split("-")
        for key in keys:
            if key.endswith("_l"):
                key = key[:-2]
            if key.endswith("_r"):
                key = key[:-2]
        return "+".join(keys).title()

    @QtCore.pyqtSlot(str, result=str)
    def toHumanThemeName(self, sequence):
        keys = sequence.split("-")
        return " ".join(keys).title()

    @QtCore.pyqtSlot(QtCore.QVariant, bool, result=QtCore.QVariant)
    def sortArray(self, data, reverse=False):
        utils.quicksort(data)
        if reverse:
            data.reverse()
        return data

    @QtCore.pyqtSlot(QtCore.QVariant, result=QtCore.QVariant)
    def sortSearchResult(self, data):
        data.sort(key=lambda d: d[1])
        return data
