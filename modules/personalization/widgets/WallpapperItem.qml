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

Item {
    id: themeItem

    property string selectedItemValue: ""
    property string itemValue: item_value

    property bool hovered: false
    property bool selected: selectedItemValue == itemValue
    property bool isDeletable: typeof(is_deletable) != "undefined" ? is_deletable : false
    property bool hoverHightlight: hovered || deleteButton.state != ""

    signal selectAction(string itemValue)
    signal deleteAction(string itemValue)

    Rectangle{
        id: selectedBackground
        width: parent.width - 6
        height: parent.height - 6
        anchors.centerIn: parent
        color: "#101010"
        radius: 3
        visible: parent.selected
    }

    Rectangle {
        id: itemThumbnailBox
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        width: 130
        color: "transparent"
        border.width: 1
        border.color: {
            if(parent.selected){
                return dconstants.activeColor
            }
            else if(parent.hoverHightlight){
                return Qt.rgba(1, 1, 1, 0.6)
            }
            else{
                return Qt.rgba(1, 1, 1, 0.3)
            }
        }

        Image {
            id: itemThumbnailImage
            anchors.centerIn: parent
            source: "file://" + item_img_url
            width: parent.width - 2
            height: parent.height - 2
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            selectAction(itemValue)
        }

        onEntered: {
            parent.hovered = true
        }

        onExited: {
            parent.hovered = false
        }
    }

    DImageButton {
        id: deleteButton
        visible: isDeletable && parent.hoverHightlight && !selected
        anchors.right: parent.right
        normal_image: "images/delete_normal.png"
        hover_image: "images/delete_hover.png"
        press_image: "images/delete_press.png"
        onStateChanged: {
            if(state){
                mouseArea.cursorShape = Qt.PointingHandCursor
            }
            else{
                mouseArea.cursorShape = Qt.ArrowCursor
            }
        }
        onClicked: deleteAction(itemValue)
    }
}
