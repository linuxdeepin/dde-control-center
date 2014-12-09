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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: timeBox
    width: parent.width
    height: 118
    color: "#1a1b1b"

    property bool use24Hour: false
    property bool editable: true
    property bool inEdit: false

    function showTip(){
        dayText.opacity = 0
        timeChangeTip.opacity = 1
        if(!timeoutHideTimeChangeTip.running){
            timeoutHideTimeChangeTip.start()
        }
    }

    function showTimeEdit(){
        showTimeEditAnimation.start()
        timeEdit.init()
        timeBox.height += buttonArea.height
        inEdit = true
    }

    function showTimeNormal(){
        showTimeNormalAnimation.start()
        timeBox.height -= buttonArea.height
        inEdit = false
    }

    Behavior on height{
        PropertyAnimation { duration: 200 }
    }

    SequentialAnimation{
        id: showTimeEditAnimation
        PropertyAnimation{ target: normalTimeBox; property: "opacity"; to: 0; duration: 200 }
        PropertyAnimation{ target: editTimeBox; property: "opacity"; to: 1; duration: 200 }
    }

    SequentialAnimation{
        id: showTimeNormalAnimation
        PropertyAnimation{ target: editTimeBox; property: "opacity"; to: 0; duration: 200 }
        PropertyAnimation{ target: normalTimeBox; property: "opacity"; to: 1; duration: 200 }
    }

    Timer{
        id: timeoutHideTimeChangeTip
        repeat: false
        running: false
        interval: 3000
        onTriggered: {
            dayText.opacity = 1
            timeChangeTip.opacity = 0
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            if(timeBox.editable){
                showTip()
            }
        }

        onDoubleClicked: {
            if(timeBox.editable){
                showTimeEdit()
            }
        }
    }

    Item {
        id: normalTimeBox
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        anchors.topMargin: 24
        visible: opacity != 0
        opacity: 1

        TimeNormal {
            id: dynamicTime
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            use24Hour: timeBox.use24Hour
        }

        DLabel {
            id: amPmText
            anchors.left: dynamicTime.right
            anchors.bottom: dynamicTime.bottom
            anchors.bottomMargin: -2
            color: "#666"

            font.pixelSize: 14
            font.family: fixedFont.name
            font.bold: true
            visible: opacity != 0
            text: globalDate.getHours() < 12 ? "AM" : "PM"
            opacity: timeBox.use24Hour ? 0 : 1

            Behavior on opacity{
                PropertyAnimation { duration: 200 }
            }
        }

        DLabel {
            id: dayText
            anchors.top: dynamicTime.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: dynamicTime.horizontalCenter

            font.pixelSize: 12
            text: globalDate.toLocaleDateString(locale)
            visible: opacity != 0

            Behavior on opacity{
                PropertyAnimation { duration: 200 }
            }
        }

        DLabel {
            id: timeChangeTip
            anchors.top: dynamicTime.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: dynamicTime.horizontalCenter
            font.pixelSize: 12
            color: dconstants.tuhaoColor
            text: dsTr("Double-click this area to change your time")
            visible: opacity != 0
            opacity: 0

            Behavior on opacity{
                PropertyAnimation { duration: 200 }
            }
        }

    }

    Item {
        id: editTimeBox
        anchors.fill: parent
        visible: opacity != 0
        opacity: 0

        TimeEdit{
            id: timeEdit
            onAccepted: {
                var year = globalDate.toLocaleDateString(locale,"yyyy")
                var month = globalDate.toLocaleDateString(locale,"MM")
                var date = globalDate.toLocaleDateString(locale,"dd")
                var second = globalDate.toLocaleTimeString(locale,"ss")

                gDate.SetDate(year,month,date,timeEdit.currentHour,timeEdit.currentMin,second,"0")

                showTimeNormal()
            }
            height: parent.height - buttonArea.height
        }

        Row{
            id: buttonArea
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: childrenRect.width
            height: childrenRect.height
            spacing: 10

            DTextAction {
                text: dsTr("Set")
                onClicked: {
                    var year = globalDate.toLocaleDateString(locale,"yyyy")
                    var month = globalDate.toLocaleDateString(locale,"MM")
                    var date = globalDate.toLocaleDateString(locale,"dd")
                    var second = globalDate.toLocaleTimeString(locale,"ss")

                    print ("==> Set Date:",year,month,date,timeEdit.currentHour,timeEdit.currentMin,second,0)
                    gDate.SetDate(year,month,date,timeEdit.currentHour,timeEdit.currentMin,second,"0")

                    timeBox.showTimeNormal()
                }
            }

            Rectangle {
                width: 1
                height: cancelButton.height - 10
                anchors.verticalCenter: cancelButton.verticalCenter
                color: dconstants.fgColor
            }

            DTextAction {
                id: cancelButton
                text: dsTr("Cancel")
                onClicked: {
                    timeBox.showTimeNormal()
                }
            }
        }
    }
}
