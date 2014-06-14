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

    signal selectAction(string itemValue)
    signal previewAction(string itemValue)

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
        width: 130
        height: 74
        color: "transparent"
        border.width: 1
        border.color: {
            if(parent.selected){
                return dconstants.activeColor
            }
            else if(parent.hovered){
                return dconstants.hoverColor
            }
            else{
                return dconstants.fgColor
            }
        }

        Image {
            id: itemThumbnailImage
            anchors.centerIn: parent
            source: item_img_url
            width: parent.width - 2
            height: parent.height - 2
            cache: false
        }
    }

    DssH2 {
        id: itemNameArea
        anchors.top: itemThumbnailBox.bottom
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        text: item_name
        color: {
            if(parent.selected){
                return dconstants.activeColor
            }
            else if(hovered){
                return dconstants.hoverColor
            }
            else{
                return dconstants.fgColor
            }
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

    Item {
        id: zoomButtonBox
        visible: parent.hovered && previewExists
        anchors.top: itemThumbnailBox.top
        anchors.topMargin: 3
        anchors.right: itemThumbnailBox.right
        anchors.rightMargin: 3
        width: zoomButton.width 
        height: zoomButton.height
        property bool previewExists: themeObj.preview ? themeObj.preview.length != 0 : false
        //property bool previewExists: true

        ShowPreviewButton {
            id: zoomButton
            source: "images/preview.png"
            anchors.centerIn: parent
            onClicked: previewAction(itemValue)
            onStateChanged: {
                themeItem.hovered = (state == "hovered")
            }
        }
    }
}
