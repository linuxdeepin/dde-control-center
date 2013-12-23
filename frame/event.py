#! /usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2011 ~ 2012 Deepin, Inc.
#               2011 ~ 2012 Wang Yong
# 
# Author:     Wang Yong <lazycat.manatee@gmail.com>
# Maintainer: Wang Yong <lazycat.manatee@gmail.com>
#             Kaisheng Ye <kaisheng.ye@gmail.com>
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

from PyQt5.QtCore import pyqtSignal, QThread
from Xlib import X
#from ocr import ocr_word
from threading import Timer
from xutils import record_event, check_valid_event, get_event_data

press_ctrl = False

class RecordEvent(QThread):

    enter_mouse_area = pyqtSignal()
    click_outer_area = pyqtSignal(int, int)
    
    def __init__(self, view):
        QThread.__init__(self)

        self.timer = None
        self.stop_delay = 0.1
        self.view = view
        self.viewHoverPadding = 15
    
    def record_callback(self, reply):
        global press_ctrl
        
        check_valid_event(reply)
     
        data = reply.data
        while len(data):
            event, data = get_event_data(data)

            if event.type == X.MotionNotify:
                if self.timer:
                    self.timer.cancel()
                self.timer = Timer(self.stop_delay, lambda : self.emit_cursor_stop(event.root_x, event.root_y))
                self.timer.start()
            elif event.type == X.ButtonRelease:
                self.click_outer_area.emit(event.root_x, event.root_y)
                
    def emit_cursor_stop(self, mouse_x, mouse_y):
        screen_size = self.view.screen().size()
        if mouse_x >= screen_size.width() - self.viewHoverPadding and \
            mouse_x <= screen_size.width() and \
            mouse_y >= screen_size.height() - self.viewHoverPadding and \
            mouse_y <= screen_size.height():
            self.enter_mouse_area.emit()
                
    def run(self):
        record_event(self.record_callback)
