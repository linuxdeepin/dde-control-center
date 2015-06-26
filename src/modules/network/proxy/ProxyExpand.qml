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

import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id: rootColumn
    width: parent.width

    DBaseExpand {
        id: rootExpand

        expanded: false
        
        header.sourceComponent: DDownArrowHeader{
            text: dsTr("System Proxy")
            onClicked: {
                rootExpand.expanded = active
            }
        }

        content.sourceComponent: Rectangle {
            color: DPalette.contentBgColor
            width: parent.width
            height: childrenRect.height

            ProxyContent {}
        }
    }
    DSeparatorHorizontal {}
}
