#! /usr/bin/python
# -*- coding: utf-8 -*-

import os
from constants import ROOT_LOCATION
from PyQt5.QtCore import QSize
from PyQt5 import QtGui
from PyQt5.QtQuick import QQuickView
from PyQt5.QtCore import pyqtSlot
from PyQt5.QtGui import QSurfaceFormat, QColor
from PyQt5 import QtCore, QtQuick

class MessageDialog(QQuickView):

    def __init__(self):
        QQuickView.__init__(self)
        surface_format = QSurfaceFormat()
        surface_format.setAlphaBufferSize(8)
        
        self.setColor(QColor(0, 0, 0, 0))
        self.setFlags(QtCore.Qt.FramelessWindowHint)
        self.setResizeMode(QtQuick.QQuickView.SizeRootObjectToView)
        self.setFormat(surface_format)
        
        self.qml_context = self.rootContext()

        self.qml_context.setContextProperty("messageWindow", self)

    @pyqtSlot(str)
    def loadQml(self, url):
        self.setSource(QtCore.QUrl.fromLocalFile(url))
        
    @pyqtSlot(result="QVariant")    
    def getCursorPos(self):
        return QtGui.QCursor.pos()        
