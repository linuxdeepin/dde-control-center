/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 WanQing Yang
**
**  Author:     WanQing Yang <match.yangwanqing@gmail.com>
**  Maintainer: WanQing Yang <match.yangwanqing@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import Deepin.Widgets 1.0


DBaseExpand {
    id: simpleSettingArea
    expanded: header.item.active

    property int itemHeight: 70

    header.sourceComponent: DDownArrowHeader {
        text: dsTr("Display Modes")
        active: true

        DSeparatorHorizontal {anchors.bottom: parent.bottom; visible: !parent.active}
    }

    content.sourceComponent: Column {
        width: parent.width
        height: childrenRect.height
        spacing: 8
        Item {height: 1;width: parent.width}//just for spacing

        //copy mode
        SimpleModelSelectItem {
            id:copyItem
            width: parent.width
            height: itemHeight
            normalImage: "images/copy_normal.png"
            hoverImage: "images/copy_hover.png"
            pressImage: "images/copy_active.png"
            itemTitle: dsTr("Copy")
            itemDetail: dsTr("Copy the contents of your primary screen to other screens.")
            actived: displayId.displayMode == 1

            onClicked: {
                displayId.SwitchMode(1,"")
                displayId.SaveChanges()
            }
        }

        //extend mode
        SimpleModelSelectItem {
            id:extendItem
            width: parent.width
            height: itemHeight
            normalImage: "images/extend_normal.png"
            hoverImage: "images/extend_hover.png"
            pressImage: "images/extend_active.png"
            itemTitle: dsTr("Extend")
            itemDetail: dsTr("Extend your screen contents to display different contents on different screens.")
            actived: displayId.displayMode == 2

            onClicked: {
                displayId.SwitchMode(2,"")
                displayId.SaveChanges()
            }
        }

        Repeater {
            id:monitorRepeator

            model: realMonitorsCount

            delegate: SimpleModelSelectItem {
                width: simpleSettingArea.width
                height: itemHeight

                normalImage: "images/single_normal.png"
                hoverImage: "images/single_hover.png"
                pressImage: "images/single_press.png"
                itemTitle: dsTr("Only Displayed on %1").arg(realMonitorsList[index])
                itemDetail: dsTr("Screen contents are only displayed on %1 but not on other screens.").arg(realMonitorsList[index])
                actived: displayId.displayMode == 3 && openedMonitors.length == 1 && openedMonitors[0].name == realMonitorsList[index]
                screenIndex: index + 1

                onClicked: {
                    displayId.SwitchMode(3,realMonitorsList[index])
                    displayId.SaveChanges()
                }
            }
        }

        //customize mode
        SimpleModelSelectItem {
            id:customizeItem
            width: parent.width
            height: itemHeight
            normalImage: "images/customize_normal.png"
            hoverImage: "images/customize_hover.png"
            pressImage: "images/customize_active.png"
            itemTitle: dsTr("Custom Settings")
            itemDetail: dsTr("You can do other custom settings to your screens.")
            actived: displayId.displayMode == 0

            onClicked: {
                displayId.SwitchMode(0,"")
                displayId.SaveChanges()
                onCustomizeMode = true
            }

            DSeparatorHorizontal {anchors.bottom: parent.bottom}
        }
    }
}
