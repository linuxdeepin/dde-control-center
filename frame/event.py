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

import xcb
import xcb.xproto
from Xlib import X, display
from Xlib.ext import record
from Xlib.protocol import rq
from threading import Timer

conn = xcb.connect()
screen = conn.get_setup().roots[0]
root = screen.root
screen_width = screen.width_in_pixels
screen_height = screen.height_in_pixels
        
record_dpy = display.Display()

def get_pointer_coordiante():
    pointer = conn.core.QueryPointer(root).reply()
    return (pointer.root_x, pointer.root_y)

def record_event(record_callback):
    ctx = record_dpy.record_create_context(
        0,
        [record.AllClients],
        [{
                'core_requests': (0, 0),
                'core_replies': (0, 0),
                'ext_requests': (0, 0, 0, 0),
                'ext_replies': (0, 0, 0, 0),
                'delivered_events': (0, 0),
                'device_events': (X.KeyPress, X.MotionNotify),
                'errors': (0, 0),
                'client_started': False,
                'client_died': False,
                }])
         
    record_dpy.record_enable_context(ctx, record_callback)
    record_dpy.record_free_context(ctx)

def get_event_data(data):
    return rq.EventField(None).parse_binary_value(data, record_dpy.display, None, None)
    
def check_valid_event(reply):
    if reply.category != record.FromServer:
        return 
    if reply.client_swapped:
        return
    if not len(reply.data) or ord(reply.data[0]) < 2:
        return

class RecordEvent(QThread):

    enter_mouse_area = pyqtSignal()
    click_outer_area = pyqtSignal(int, int)
    
    def __init__(self, view):
        QThread.__init__(self)

        self.stop_delay = 0.8
        self.view = view
        self.viewHoverPadding = 60
        self.timer = Timer(self.stop_delay, self.enter_mouse_area.emit)
    
    def record_callback(self, reply):
        
        check_valid_event(reply)
     
        data = reply.data
        while len(data):
            event, data = get_event_data(data)

            if event.type == X.MotionNotify:
                if self.in_emit_area(event) and not self.timer.isAlive():
                    self.timer = Timer(self.stop_delay, self.enter_mouse_area.emit)
                    self.timer.start()
                elif not self.in_emit_area(event) and self.timer.isAlive():
                    self.timer.cancel()

            elif event.type == X.ButtonRelease:
                #print(event.root_x, event.root_y)
                self.click_outer_area.emit(event.root_x, event.root_y)
                
    def in_emit_area(self, event):
        if not event:
            return False
        else:
            return event.root_x >= screen_width - 2 and \
                event.root_x <= screen_width and \
                event.root_y >= self.viewHoverPadding and \
                event.root_y <= screen_height
                
    def run(self):
        record_event(self.record_callback)
