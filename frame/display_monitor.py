#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2012~2013 Deepin, Inc.
#               2012~2013 Kaisheng Ye
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

import sys
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

from PyQt5.QtCore import (
        pyqtSlot, pyqtProperty, pyqtSignal,
        QObject, QVariant, QThread,
        QCoreApplication)
from PyQt5.QtDBus import (
        QDBusConnection,
        QDBusMessage,
        QDBusReply,
        QDBusInterface,
        QDBusAbstractInterface,
        )

DISPLAY_DBUS_SERVICE = "com.deepin.daemon.Display"
DISPLAY_DBUS_PATH = "/com/deepin/daemon/Display"
DISPLAY_DBUS_INTERFACE = DISPLAY_DBUS_SERVICE

session_bus = QDBusConnection.sessionBus()

from Xlib import X, display
from Xlib.ext import record
from Xlib.protocol import rq
from threading import Timer

record_dpy = display.Display()

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

def connect_to_primary_changed(method):
    session_bus.connect(
            DISPLAY_DBUS_SERVICE,
            DISPLAY_DBUS_PATH,
            DISPLAY_DBUS_INTERFACE,
            "PrimaryChanged",
            method)

class RecordEvent(QThread):

    enter_mouse_area = pyqtSignal()
    click_outer_area = pyqtSignal(int, int)
    
    def __init__(self, view):
        QThread.__init__(self)

        self.stop_delay = 0.5
        self.view = view
        self.viewHoverPadding = 30
        self.timer = Timer(self.stop_delay, self.enter_mouse_area.emit)

        self.display_iface = QDBusInterface(
                DISPLAY_DBUS_SERVICE,
                DISPLAY_DBUS_PATH,
                "org.freedesktop.DBus.Properties",
                session_bus,
                None)

        self.screen_rect = QDBusReply(self.display_iface.call("Get",
            DISPLAY_DBUS_INTERFACE, "PrimaryRect")).value()
        connect_to_primary_changed(self.update_screen_rect)

    @pyqtSlot(QDBusMessage)
    def update_screen_rect(self, message):
        self.screen_rect = QDBusReply(message).value()

    @pyqtProperty(QVariant)
    def primaryRect(self):
        return self.screen_rect
    
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
                #print (event.root_x, event.root_y)
                self.click_outer_area.emit(event.root_x, event.root_y)
                
    def in_emit_area(self, event):
        if not event:
            return False
        else:
            x, y, width, height = self.screen_rect
            return event.root_x >= x + width - self.viewHoverPadding and \
                event.root_x <= x + width and \
                event.root_y >= y + height - self.viewHoverPadding and \
                event.root_y <= y + height
                
    def run(self):
        record_event(self.record_callback)

