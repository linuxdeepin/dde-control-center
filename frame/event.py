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

from PyQt5.QtCore import QObject, pyqtSignal
from Xlib import X
#from ocr import ocr_word
from threading import Timer
from xutils import record_event, get_keyname, check_valid_event, get_event_data
import commands, subprocess

press_ctrl = False

class RecordEvent(QObject):
    
    press_ctrl = pyqtSignal()    
    release_ctrl = pyqtSignal()    
    
    left_button_press = pyqtSignal(int, int, int)
    right_button_press = pyqtSignal(int, int, int)    
    wheel_press = pyqtSignal()
    
    cursor_stop = pyqtSignal(int, int, str)
    
    translate_selection = pyqtSignal(int, int, str)
    
    def __init__(self, view):
        QObject.__init__(self)

        self.timer = None
        self.stop_delay = 0.8
        self.view = view
        self.viewHoverPadding = 100
    
    def record_callback(self, reply):
        global press_ctrl
        
        check_valid_event(reply)
     
        data = reply.data
        while len(data):
            event, data = get_event_data(data)
            
            """
            if event.type == X.KeyPress:
                keyname = get_keyname(event)
                if keyname in ["Control_L", "Control_R"]:
                    press_ctrl = True
                    
                    if not self.view.in_translate_area():
                        self.press_ctrl.emit()
            elif event.type == X.KeyRelease:
                keyname = get_keyname(event)
                if keyname in ["Control_L", "Control_R"]:
                    press_ctrl = False
                    self.release_ctrl.emit()
            elif event.type == X.ButtonPress:
                if event.detail == 1:
                    self.left_button_press.emit(event.root_x, event.root_y, event.time)
                elif event.detail == 3:
                    self.right_button_press.emit(event.root_x, event.root_y, event.time)
                elif event.detail == 5:
                    self.wheel_press.emit()
            elif event.type == X.ButtonRelease:
                if not self.view.in_translate_area():
                    selection_content = commands.getoutput("xsel -p -o")
                    subprocess.Popen("xsel -c", shell=True).wait()
                    
                    if len(selection_content) > 1:
                        self.translate_selection.emit(event.root_x, event.root_y, selection_content)
            elif event.type == X.MotionNotify:
            """
            if event.type == X.MotionNotify:
                if self.timer:
                    self.timer.cancel()
                self.timer = Timer(self.stop_delay, lambda : self.emit_cursor_stop(event.root_x, event.root_y))
                self.timer.start()
            elif event.type == X.ButtonRelease:
                self.view.rootObject().clickOutArea.emit(event.root_x, event.root_y)
                
    def emit_cursor_stop(self, mouse_x, mouse_y):
        screen_size = self.view.screen().size()
        if mouse_x >= screen_size.width() - 2 and \
            mouse_x <= screen_size.width() and \
            mouse_y >= self.viewHoverPadding and \
            mouse_y <= screen_size.height() - self.viewHoverPadding:
            self.view.rootObject().enterMouseArea.emit()
                
    def filter_event(self):
        record_event(self.record_callback)
