#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2011 ~ 2012 Deepin, Inc.
#               2011 ~ 2012 Wang Yong
# 
# Author:     Wang Yong <lazycat.manatee@gmail.com>
# Maintainer: Wang Yong <lazycat.manatee@gmail.com>
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

import subprocess
import os
from ConfigParser import RawConfigParser as ConfigParser

if __name__ == "__main__":
    # Read config options.
    config_parser = ConfigParser()
    config_parser.read("locale_config.ini")
    project_name = config_parser.get("locale", "project_name")
    source_dir = config_parser.get("locale", "source_dir")
    locale_dir = os.path.abspath(config_parser.get("locale", "locale_dir"))
    langs = eval(config_parser.get("locale", "langs"))
    
    # Get input arguments.
    source_files = []
    for root, dirs, files in os.walk(source_dir):
        for each_file in files:
            if each_file.endswith(".py") and not each_file.startswith("."):
                source_files.append(os.path.join(root, each_file))
    
    ts_filepath = os.path.join(locale_dir, project_name + ".ts")
    pot_filepath = os.path.join(locale_dir, project_name + ".pot")

    # Generate ts file 
    subprocess.call(
        "deepin-lupdate -recursive %s -ts %s" % (os.path.realpath(source_dir), ts_filepath),
        shell=True)
    
    # Generate pot file.
    subprocess.call(
        "lconvert -i %s -o %s" % (ts_filepath, pot_filepath),
        shell=True)

    clean_str = ""
    with open(pot_filepath) as fp:
        for line in fp:
            if not line.startswith("msgctxt"):
                clean_str += line

    with open(pot_filepath, "wb") as fp:
        fp.write(clean_str)

    subprocess.call(
        "xgettext -k_ -j -o %s %s" % (pot_filepath, ' '.join(source_files)),
        shell=True)

    
    # Update po files from pot files.
    for lang in langs:
        subprocess.call(
            "msgmerge -U %s %s" % (
                os.path.join(locale_dir, "%s.po" % (lang)),
                os.path.join(locale_dir, "%s.pot" % (project_name))
                ),
            shell=True
            )
