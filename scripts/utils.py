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

def quicksort(data, low = 0, high = None):
    if high == None:
        high = len(data) - 1
    if low < high:
        s, i, j = data[low], low, high
        while i < j:
            while i < j and data[j] >= s:
                j = j - 1
            if i < j:
                data[i] = data[j]
                i = i + 1
            while i < j and data[i] <= s:
                i = i + 1
            if i < j:
                data[j] = data[i]
                j = j - 1
        data[i] = s
        quicksort(data, low, i - 1)
        quicksort(data, i + 1, high)

