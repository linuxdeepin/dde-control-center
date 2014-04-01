#! /usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt5 import QtCore, QtQuick

class PopupItem(QtQuick.QQuickItem):
    
    _windowViewSignal = QtCore.pyqtSignal()
    _parentObjectSignal = QtCore.pyqtSignal()
    _rootWindowSignal = QtCore.pyqtSignal()
    
    def __init__(self, parent=None):
        super(PopupItem, self).__init__(parent)
        
        self._windowView = None
        self._rootWindow = None
        self._parentObject = None
        
    @QtCore.pyqtProperty("QVariant", notify=_windowViewSignal)    
    def windowObject(self):    
        return self._windowView
    
    @windowObject.setter
    def windowObject(self, view):
        if self._windowView is None:
            self._windowView = view
            self._windowView.focusLosed.connect(self.onWindowFocusLosed)

    @QtCore.pyqtProperty("QVariant", notify=_rootWindowSignal)
    def rootWindowObject(self):    
        return self._rootWindow
    
    @rootWindowObject.setter
    def rootWindowObject(self, obj):
        self._rootWindow = obj
        self._rootWindow.mousePressed.connect(self.onParentMousePressed)
            
    @QtCore.pyqtProperty("QVariant", notify=_parentObjectSignal)    
    def parentObject(self):    
        return self._parentObject
    
    @parentObject.setter
    def parentObject(self, obj):
        self._parentObject = obj
        
    def onParentMousePressed(self, point):    
        rootPoint =  self.mapToItem(None, QtCore.QPointF(0, 0))
        rect = QtCore.QRectF(rootPoint, QtCore.QSizeF(self.width(), self.height()))
        if not rect.contains(point):
            self.parentObject.setVisible(False)
            
    def onWindowFocusLosed(self):        
        self.parentObject.setVisible(False)
