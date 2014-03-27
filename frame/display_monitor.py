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

from PyQt5.QtDBus import QDBusConnection

DISPLAY_DBUS_SERVICE = "com.deepin.daemon.Display"
DISPLAY_DBUS_PATH = "/com/deepin/daemon/Display"
DISPLAY_DBUS_INTERFACE = DISPLAY_DBUS_SERVICE

session_bus = QDBusConnection.sessionBus()

def connect_to_primary_changed(method):
    session_bus.connect(
            DISPLAY_DBUS_SERVICE,
            DISPLAY_DBUS_PATH,
            DISPLAY_DBUS_INTERFACE,
            "PrimaryChanged",
            method
    )
