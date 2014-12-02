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

import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    width: 200
    height: 500

    color: "black"
    DSelectView {
        width: 200
        radioMode: false

        model: ListModel {
            ListElement {
                itemId: "1"
                itemText: "one"
                itemWidth: 0
            }
            ListElement {
                itemId: "2"
                itemText: "two"
                itemWidth: 0
            }
            ListElement {
                itemId: "3"
                itemText: "hello world"
                itemWidth: 0
            }
            ListElement {
                itemId: "4"
                itemText: "linux deepin"
                itemWidth: 0
            }
            ListElement {
                itemId: "5"
                itemText: "five five five"
                itemWidth: 0
            }
        }
    }
}
