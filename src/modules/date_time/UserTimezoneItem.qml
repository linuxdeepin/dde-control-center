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
import "../shared"

Item {
    id: userItem
    width: parent.width
    height: messageItem.height

    property string pTimezone: timezone
    property int pTimezoneOffset: getOffsetByZone(pTimezone)

    property string pOffsetName: getOffsetName(pTimezoneOffset)
    property var pDSTList: getDSTByZone(pTimezone)
    property bool pHasDST: {
        var tmpArray = Object.keys(pDSTList)
        if (tmpArray.length < 1)
            return false
        else if (tmpArray.length == 1){
            if (tmpArray[0] == 0)
                return false
            else
                return true
        }
        else
            return true
    }

    property bool pIsCurrentTimezone: pTimezoneOffset == currentTimezoneOffset
    property bool pInDeleteAction : inDeleteAction

    signal selectAction(string timezone)
    signal deleteAction(int timezoneOffset)

    Rectangle {
        visible: pIsCurrentTimezone
        anchors.fill: parent
        height: parent.height
        width: parent.width
        color:"#0D0D0D"
    }

    Item {
        id:messageItem
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 50

        Image {
            id:selectImg
            width: 14
            height: 14
            source: "qrc:/modules/date_time/images/select.png"
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            visible: pIsCurrentTimezone
        }

        DssMultiDeleteButton {
            id: deleteButton
            width: 16
            height: 16
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            opacity: {
                if (pIsCurrentTimezone)
                    return 0
                else{
                    if (pInDeleteAction){
                        return 1
                    }
                    else{
                        return 0
                    }
                }
            }

            visible: opacity != 0

            onClicked: {
                deleteAction(pTimezoneOffset)
            }

            Behavior on opacity {
                PropertyAnimation {duration: 150}
            }
        }

        DssH3 {
            id: cityText
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: deleteButton.right
            anchors.leftMargin: 14
            color: pIsCurrentTimezone ? dconstants.activeColor : "#e6e6e6"
            font.pixelSize: 12
            elide: Text.ElideRight
            height: 30
            width: parent.width - deleteButton.width - 36
            Component.onCompleted: {
                text = reduceTextLengthWithFlag(getCityListByOffset(pTimezoneOffset),font.pixelSize,width,",")
            }
        }

        DssH3 {
            id:offsetText
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.left: deleteButton.right
            anchors.leftMargin: 14
            text: pOffsetName
            color: pIsCurrentTimezone ? dconstants.activeColor : "#e6e6e6"
            font.pixelSize: 11
            elide: Text.ElideRight
            height: 20
            width: parent.width - deleteButton.width - 36

        }

        MouseArea {
            anchors.fill: parent
            anchors.leftMargin: 26
            onClicked: {
                if (!pInDeleteAction)
                    selectAction(pTimezone)
            }
        }

    }

    DSeparatorHorizontal {anchors.bottom: parent.bottom}
}
