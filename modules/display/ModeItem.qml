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
import QtQuick.Controls 1.1

Rectangle {
    id: delegate

    width: delegate.GridView.view.cellWidth
    height: delegate.GridView.view.cellHeight
    color: Qt.rgba(0, 0, 0, 0)

    property int centerSpacing: 3
    property int verticalPadding: 3
    property int horizontalPadding: 10

    property color fontNormalColor: dconstants.fgColor
    property color fontHoverColor: dconstants.hoverColor
    property color fontPressedColor: dconstants.activeColor
    property color bgNormalColor: Qt.rgba(0, 0, 0, 0)
    property color bgPressedColor: Qt.rgba(0, 0, 0, 0.4)

    // set from model
    property string itemLabel: item_label
    property int itemValue: item_value
    // set from model

    property var modesDict
    property string currentValue: "0x0" // set from delegate

    property bool selected: {
        if(itemLabel){
            return getIndexFromArray(currentValue, modesDict[itemLabel]) != -1
        }
        else{
            return false
        }
    }
    property bool hovered: false

    Component.onCompleted: {
    }

    signal selectAction(int itemValue)

    Rectangle {
        id: contentBox
        anchors.centerIn: parent
        width: horizontalPadding * 2 + txt.implicitWidth
        height: verticalPadding * 2 + txt.height + centerSpacing
        color: delegate.selected ? bgPressedColor : bgNormalColor
        radius: 4

        Label {
            id: txt
            anchors.centerIn: parent
            color: {
                if(delegate.selected){
                    return fontPressedColor
                }
                else if(delegate.hovered){
                    return fontHoverColor
                }
                else{
                    return fontNormalColor
                }
            }
            text: itemLabel
            font.pixelSize: 12
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            parent.hovered = true
        }

        onExited: {
            parent.hovered = false
        }

        onClicked: {
            parent.selectAction(parent.itemValue)
        }
    }
}
