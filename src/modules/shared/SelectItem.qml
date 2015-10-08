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
    id: selectItem
    width: parent.width
    height: 28
    property int labelLeftMargin: 44
    property bool selected: selectItemId == itemId
    property bool hovered: false
    
    property bool showTip: nameText.isElide

    // info from model
    property string itemId: typeof(item_id) == "undefined" ? "" : item_id
    property string itemName: typeof(item_name) == "undefined" ? "" : item_name
    // info from model

    // Need binding variable
    property int totalItemNumber: 0
    property string selectItemId: ""
    property bool inDeleteAction: false
    // Need binding variable

    signal deleteAction(string itemId)
    signal selectAction(string itemId)

    Rectangle {
        visible: selected && totalItemNumber != 1
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: 28
        color: "#0D0D0D"
        radius: 4
    }

    Image {
        id: nameImage
        anchors.left: parent.left
        anchors.leftMargin: 18
        width: deleteButton.width
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: nameText.left
        source: "images/select.png"
    }

    DssMultiDeleteButton {
        id: deleteButton
        anchors.left: nameImage.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        opacity: {
            if(selected){
                nameImage.visible = (totalItemNumber != 1)
                return 0
            }
            else{
                nameImage.visible = false
                if(inDeleteAction){
                    return 1
                }
                else{
                    return 0
                }
            }
        }

        visible: opacity != 0

        onClicked: {
            deleteAction(itemId)
        }

        Behavior on opacity {
            PropertyAnimation {duration: 150}
        }
    }
    
    DssH3 {
        id: nameText
        width: parent.width - nameImage.width - deleteButton.width
        anchors.left: parent.left
        anchors.leftMargin: labelLeftMargin
        anchors.verticalCenter: parent.verticalCenter
        text: itemName
        //elide: Text.ElideRight

        property bool isElide: false

        color: {
            if(selected){
                return DPalette.activeColor
            }else if(hovered){
                return DPalette.hoverColor
            }
            else{
                return DPalette.fgColor
            }
        }
        font.pixelSize: 12

        Component.onCompleted: {
            if(width < contentWidth){
                elide = Text.ElideRight
                isElide = true
            }
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        visible: !inDeleteAction
        
        onEntered: {
            if (selectItem.showTip) toolTip.showTip(itemName)
            hovered = true
        }

        onExited: {
            toolTip.hideTip()
            hovered = false
        }
        
        onClicked: {
            selectAction(itemId)
        }
    }

    ListView.onAdd: SequentialAnimation {
        PropertyAction { target: selectItem; property: "height"; value: 0 }
        NumberAnimation { target: selectItem; property: "height"; to: 28; duration: 250; easing.type: Easing.InOutQuad }
    }

    ListView.onRemove: SequentialAnimation {
        PropertyAction { target: selectItem; property: "ListView.delayRemove"; value: true }
        NumberAnimation { target: selectItem; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }

        // Make sure delayRemove is set back to false so that the item can be destroyed
        PropertyAction { target: selectItem; property: "ListView.delayRemove"; value: false }
    }

}
