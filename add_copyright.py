#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os

copyright_str = """\
/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

"""

exts = [".cpp", ".h", ".qml"]

def scan(root_folder):
    for (root, folders, files) in os.walk(root_folder):
        for f in files:
            path = os.path.join(root, f)
            ext = os.path.splitext(path)[1]
            if ext in exts:
                f = open(path)
                old_content = f.read()
                if copyright_str in old_content:
                    f.close()
                    continue
                new_content = copyright_str + old_content
                f.close()
                f = open(path, "w")
                f.write(new_content)
                f.close()

if __name__ == "__main__":
    import sys
    scan(sys.argv[1])

