#! /usr/bin/env python
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

modules_id = [
"notification",            #系统消息
"display",                 #显示
"desktop",                 #桌面
"individuation",           #个性化
"sound",                   #声音
"date_time",               #日期和时间
"power",                   #电源
"shortcuts",               #快捷键
"keyboard",                #键盘
#"mouse",                   #鼠标*
"touchpad",                #触摸板
"network",                 #网络
"bluetooth",               #蓝牙
"disk_mount",              #磁盘挂载
"account",                 #账户设置
"default_applications",    #默认程序
"system_info",             #系统信息
#"shutdown",                #关机
"dss",
]

@pyqtSlot(result=QVariant)
def get_modules_id():
    return modules_id

class ModulesInfo(QObject):
    def __init__(self):
        QObject.__init__(self)

    def module_id(self):
        pass
    
    def qml_path(self):
        pass

    def icon_path(self):
        pass

