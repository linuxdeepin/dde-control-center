/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <yangwanqing@linuxdeepin.com>
**  Maintainer: Wanqing Yang <yangwanqing@linuxdeepin.com>
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

Item {
    id:deviceItem
    width: parent.width
    height: 30

    property bool isConnected:devInfo.Connected
    property string itemId: typeof(item_id) == "undefined" ? "" : item_id
    property string itemName: typeof(item_name) == "undefined" ? "" : item_name

    signal itemClicked(string id, string name, bool state)

    DImageButton {
        id: checkImg
        width: 14
        height: 14
        anchors {verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 15}
        normal_image: getIconUrl("bluetooth/connected.png")
        hover_image: getIconUrl("bluetooth/disconnect.png")
        visible: isConnected
        onClicked: {
            deviceItem.itemClicked(itemId,itemName,isConnected)
        }
    }

    DssH2 {
        id:deviceText
        text: itemName
        width: text.length
        height: parent.height
        color: "#bcbcbc"

        verticalAlignment: Text.AlignVCenter
        anchors {left: checkImg.right; leftMargin: 5}
    }

    MouseArea {
        anchors.fill: parent
        anchors.leftMargin: checkImg.width + 15
        hoverEnabled: true
        onEntered: {
            deviceText.color = "#ffffff"
        }
        onExited: deviceText.color = "#bcbcbc"

        onClicked: deviceItem.itemClicked(itemId,itemName,isConnected)
    }

}
