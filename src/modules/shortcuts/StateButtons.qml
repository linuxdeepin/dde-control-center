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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import "../shared"

Item {
    id: stateButtons
    width: actionButtons.width
    height: parent.height

    property alias addButton: addButton
    property alias deleteButton: deleteButton
    property alias completeButton: completeButton

    property string completeButtonLabel: dsTr("Done")
    property string currentActionStateName: ""

    DTextButton {
        id: completeButton
        text: completeButtonLabel
        opacity: stateButtons.currentActionStateName == "deleteButton" ? 1 : 0
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        visible: opacity == 0 ? false : true

        Behavior on opacity {
            PropertyAnimation { duration:200 }
        }

        onClicked: {
            stateButtons.currentActionStateName = ""
        }
    }


    ItemTooltip {
        id:shortcutsTooltip
        anchors.right: actionButtons.left
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        width: 200
        delayShowInterval: 500
        textItem.font.pixelSize: 12
    }

    Row {
        id: actionButtons
        anchors.verticalCenter: parent.verticalCenter
        width: childrenRect.width
        spacing: 4
        opacity: stateButtons.currentActionStateName == "" ? 1 : 0
        visible: opacity == 0 ? false : true

        Behavior on opacity {
            PropertyAnimation { duration:200 }
        }

        DssDeleteButton {
            id: deleteButton
            onClicked: {
                stateButtons.currentActionStateName = "deleteButton"
                shortcutsTooltip.hideTipImmediately()
            }
            onStateChanged: {
                if (state == "hovered" && visible){
                    shortcutsTooltip.tooltip = dsTr("Delete Shortcut")
                    shortcutsTooltip.showTip()
                }

                else
                    shortcutsTooltip.hideTip()
            }
        }

        DssAddButton {
            id: addButton
            onClicked: {
                stateButtons.currentActionStateName = "addButton"
                shortcutsTooltip.hideTipImmediately()
            }
            onStateChanged: {
                if (state == "hovered" && visible){
                    shortcutsTooltip.tooltip = dsTr("Add Shortcut")
                    shortcutsTooltip.showTip()
                }
                else
                    shortcutsTooltip.hideTip()
            }
        }
    }

}
