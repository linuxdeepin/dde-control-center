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

from PyQt5.QtCore import QObject, pyqtSlot, QVariant
from nls import _

"""
翻译po
"""

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
                "notification": _("System Information"),
                "display": "显示",
                "desktop": "桌面",
                "individuation": "个性化",
                "shortcuts": "快捷键",
                "keyboard": "键盘",
                #"mouse": "",
                #"autostart": "", #开机启动, 系统服务
                "touchpad": "触摸板",
                "account": "账户",
                "default_applications": "默认程序",
                "system_info": "系统信息",
            }

        self._common_ids = [
                "date_time",
                "notification",
                "network",
                "disk_mount",
                "bluetooth",
                "sound",
                "power",
                "dss",
                "shutdown",
            ]

        self._hide_ids = [
                "date_time",
                "display",
                "desktop",
                "individuation",
                "shortcuts",
                "keyboard",
                #"mouse",
                "touchpad",
                "account",
                "default_applications",
                "system_info",
            ]

    @pyqtSlot(result=QVariant)
    def common_ids(self):
        return self._common_ids

    @pyqtSlot(result=QVariant)
    def hide_ids(self):
        return self._hide_ids

    @pyqtSlot(result=QVariant)
    def module_names(self):
        return self._l18n_names

