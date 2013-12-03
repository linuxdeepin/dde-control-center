#!/usr/bin/env python
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

from PyQt5.QtCore import QObject, pyqtSlot, QVariant, pyqtProperty
from nls import _

class ModulesId(QObject):
    def __init__(self):
        QObject.__init__(self)
        self._l18n_names = {
                "date_time": _("Date & Time"),
                "network": _("Network"),
                "disk_mount": _("Disk Mount"),
                "bluetooth": _("Bluetooth"),
                "sound": _("Sound"),
                "power": _("Power"),
                "dss": _("More Settings"),
                "shutdown": _("Shutdown"),
                "notification": _("Notification"),
                "display": _("Display"),
                "desktop": _("Desktop"),
                "individuation": _("Individuation"),
                "shortcuts": _("Shortcuts"),
                "keyboard": _("Keyboard"),
                "mouse": _("Mouse"),
                #"autostart": "", #开机启动, 系统服务
                "touchpad": _("Touchpad"),
                "account": _("Account"),
                "default_applications": _("Default Applications"),
                "system_info": _("System Information"),
            }

        self._states_info = {
                "network": ['wifi', 'wire'],
            }

        self._common_ids = [
                "notification",
                "network",
                "disk_mount",
                "bluetooth",
                "sound",
                "power",
                "system_info",
                "dss",
                "shutdown",
            ]

        self._hide_ids = [
                "date_time",
                "desktop",
                "individuation",
                "mouse",
                "touchpad",
                "keyboard",
                "shortcuts",
                "display",
                "account",
                "default_applications",
            ]

    @pyqtSlot(str, result=QVariant)
    def getStatesArray(self, s):
        return self._states_info.get(s, [])

    @pyqtSlot(str, result=str)
    def check_valid(self, s):
        if s in self._common_ids:
            return 'normal'
        elif s in self._hide_ids:
            return 'expand'
        else:
            return ''

    @pyqtProperty(QVariant)
    def commonIds(self):
        return self._common_ids

    @pyqtProperty(QVariant)
    def hideIds(self):
        return self._hide_ids

    @pyqtProperty(QVariant)
    def moduleLocaleNames(self):
        return self._l18n_names

