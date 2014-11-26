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
import Deepin.DockApplet 1.0
import Deepin.AppletWidgets 1.0

Item {
    id:deviceItem
    width: parent.width
    height: 30

    property bool isConnected:itemState == deviceConnected
    property string itemId: typeof(item_id) == "undefined" ? "" : item_id
    property string itemName: typeof(item_name) == "undefined" ? "" : item_name
    property int itemState: typeof(item_state) == "undefined" ? "" : item_state
    property int deviceDisconnected: 0
    property int deviceConnecting: 1
    property int deviceConnected: 2

    signal itemClicked(string id, string name, bool state)

    AppletConnectButton {
        id: checkImg
        width: 14
        height: 14
        anchors {verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 15}
        visible: isConnected
        onClicked: {
            deviceItem.itemClicked(itemId,itemName,isConnected)
        }
    }

    AppletWaitingImage {
        anchors.left: checkImg.left
        anchors.verticalCenter: parent.verticalCenter
        on: itemState == deviceConnecting
    }

    DssH2 {
        id:deviceText
        text: itemName
        width: parent.width - checkImg.width - 25
        height: parent.height
        color: "#bcbcbc"
        elide: Text.ElideRight

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
