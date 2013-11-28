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

# To test other language, use below method:
#       env LANG=zh_CN LANGUAGE=zh_CN foo.py

from deepin_utils.file import get_parent_dir
import gettext
import os
from PyQt5.QtCore import pyqtSlot, QObject

LOCALE_DIR=os.path.join(get_parent_dir(__file__, 2), "locale")
if not os.path.exists(LOCALE_DIR):
    LOCALE_DIR="/usr/share/locale"

app_id = "deepin-system-settings"

_ = None    
try:
    _ = gettext.translation(app_id, LOCALE_DIR).gettext
except Exception, e:
    try:
        _ = gettext.translation(app_id, LOCALE_DIR, ['en_US']).gettext
    except Exception, e:
        _ = lambda i : i

def get_locale_code():
    try:
        return gettext.find(app_id, LOCALE_DIR).split(LOCALE_DIR)[1].split('/')[1]
    except:
        return "en_US"

@pyqtSlot(str, result=str)
def qsTr(s):
    return _(s)

class QtGettext(QObject):
    def __init__(self):
        QObject.__init__(self)

    @pyqtSlot(str, result=str)
    def qsTr(self, s):
        return _(s)

    @pyqtSlot(result=str)
    def get_locale_code(self):
        return get_locale_code()
