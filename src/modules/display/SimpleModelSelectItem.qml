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
import Deepin.Widgets 1.0

Item {
    id:selectItem
    width: parent.width
    height: 50

    property url normalImage: ""
    property url hoverImage:""
    property url pressImage:""
    property string itemTitle:""
    property string itemDetail:""
    property bool actived: false
    property string screenIndex: ""
    property bool isHover:false
    property string textColor: {
        if (actived)
            return DPalette.activeColor
        else{
            if (isHover)
                return DPalette.hoverColor
            else
                return DPalette.fgColor
        }
    }

    signal clicked()

    Image {
        id:itemIcon
        height: 30
        width: 50
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter

        source: {
            if (actived)
                return pressImage
            else{
                if (isHover)
                    return hoverImage
                else
                    return normalImage
            }
        }

        DssH1 {
            width: parent.width
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: screenIndex
            color: textColor
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: isHover = true
            onExited: isHover = false
            onClicked: selectItem.clicked()
        }
    }

    Item {
        id:rightContentItem
        width: parent.width - itemIcon.width - 15
        height: parent.height

        anchors.left: itemIcon.right
        anchors.leftMargin: 11
        anchors.verticalCenter: parent.verticalCenter

        DLabel {
            id:itemTitleH
            width: parent.width
            anchors.bottom: itemDetailH.top
            anchors.left: parent.left
            anchors.leftMargin: 5
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignBottom
            color: textColor
            font.pixelSize: 14

            text: itemTitle
        }

        TextEdit {
            id: itemDetailH
            width: parent.width - 10
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            color: textColor
            font.pixelSize: 11
            text: itemDetail
            textMargin: 6
            wrapMode: TextEdit.WordWrap
            horizontalAlignment: TextEdit.AlignLeft
            verticalAlignment: TextEdit.AlignVCenter
            readOnly: true
        }

        DSeparatorHorizontal {
            anchors.bottom: parent.bottom
            width: parent.width
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: isHover = true
            onExited: isHover = false
            onClicked: selectItem.clicked()
        }
    }
}
