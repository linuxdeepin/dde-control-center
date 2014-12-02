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

Item {
    id: expandArea
    width: parent.width
    height: content.height

    property var modelComponent
    property var expandItems

    property int expandItemIndex: -1

    Column {
        id: content
        anchors.top: parent.top
        width: parent.width
        
        Repeater {
            id: repeater
            model: expandArea.expandItems.length
            property alias items: expandArea.expandItems
            delegate: Component {
                Column {
                    width: parent.width
                    height: repeater.count - 1 == index ? expand.height : expand.height + 2
                    DBaseExpand {
                        id: expand
                        expanded: expandArea.expandItemIndex == index
                        
                        property string currentDisplayName: ""
                        
                        onExpandedChanged: {
                            header.item.active = expanded
                        }
                        
                        header.sourceComponent: DDownArrowHeader {
                            text: expandArea.expandItems[index].name
                            icon: expandArea.expandItems[index].icon
                            width: parent.width
                            leftMargin: 25
                            rightMargin: 15
                            
                            Component.onCompleted: {
                                active = expand.expanded
                            }
                            
                            onClicked: {
                                expandArea.expandItemIndex = active ? index : -1
                            }
                        }
                        
                        content.sourceComponent: modelComponent
                        contentData: expandArea.expandItems[index]
                    }

                    DSeparatorHorizontal {}
                }
            }
        }
    }
}
