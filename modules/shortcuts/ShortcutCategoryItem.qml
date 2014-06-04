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
    width: parent.width
    height: childrenRect.height

    property string categoryName: name
    property int myIndex: index
    property var categoryKeyBindings: dbusKeyBinding[propertyName]

    DBaseExpand {
        id: expand
        expanded: expandItemIndex == myIndex

        onExpandedChanged: {
            header.item.active = expanded
        }

        header.sourceComponent: DDownArrowHeader {
            text: categoryName
            width: parent.width
            rightMargin: 15
            
            Component.onCompleted: {
                active = expand.expanded
            }
            
            onClicked: {
                expandItemIndex = active ? myIndex : -1
            }
        }
        
        content.sourceComponent: Component {
            ListView {
                id: lists
                focus: true
                width: parent.width
                height: childrenRect.height
                model: categoryKeyBindings.length
                delegate: ShortcutInput {
                    info: categoryKeyBindings[index]
                }
            }
        }
    }

    DSeparatorHorizontal {}
}
