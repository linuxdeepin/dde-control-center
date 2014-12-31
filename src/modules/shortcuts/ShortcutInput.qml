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
import "../shared/"

Item {
    id: keyBindingItem
    width: parent.width
    height: childrenRect.height
    clip: true

    property var dconstants: DConstants {}
    property int grabKeyAreaWidth: tipInfoText.width + 6

    property var info
    property int shortcutId: info[0]
    property string displayName: info[1]
    property string shortcutName: mainObject.toHumanShortcutLabel(info[2])

    property bool grabFlag: false
    property bool showDelete: false
    property int contentLeftMargin: 22
    property bool hovered: false
    property bool inConfictDealing: false

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    Connections {
        target:shortcutsModule
        onCanShowWarningChanged:{
            if (!canShowWarning){
                conflictInfoArea.height = 0
                stopSetKeyBinding = false
                inConfictDealing = false
            }
        }
    }

    Item {
        id: keyBindingArea
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 30

        DssMultiDeleteButton {
            id: deleteButton
            anchors.left: parent.left
            anchors.leftMargin: showDelete ? dconstants.leftMargin : 0 - width
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 1
            scale: 0.9

            onClicked: {
                dbusKeyBinding.DeleteCustomShortcut(shortcutId)
            }

            Behavior on anchors.leftMargin {
                PropertyAnimation {duration: 100}
            }
        }

        DssH3 {
            text: displayName
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: showDelete ? contentLeftMargin + deleteButton.width : contentLeftMargin

            Behavior on anchors.leftMargin {
                PropertyAnimation {duration: 100}
            }
        }

        Rectangle {
            width: grabKeyAreaWidth
            height: parent.height - 6
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            radius: 2
            color: "#101010"
            visible: grabFlag
        }

        Text {
            id: realShortcutName
            anchors.fill: parent
            anchors.rightMargin: 15
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 11
            color: hovered ? dconstants.hoverColor :  dconstants.fgColor
            text: shortcutName? shortcutName : dsTr("None")
            visible: {
                if(grabFlag | inConfictDealing){
                    return false
                }
                else{
                    return true
                }
            }
        }

        Text {
            id: tipInfoText
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 11
            color: dconstants.fgDarkColor
            text: keyPressed ? keyPressInfo : dsTr("Please input new shortcut")
            visible: grabFlag
            property bool keyPressed: false
            property string keyPressInfo: ""
            onKeyPressedChanged: {
                if(!keyPressed){
                    keyPressInfo = ""
                }
            }
        }

        Text {
            id: confictText
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 11
            color: "#f68a32"
            visible: inConfictDealing
        }

        Connections {
            target: dbusKeyBinding
            onKeyReleaseEvent: {
                if (currentShortcutId == shortcutId){
                    grabFlag = false
                    var arg0_lower_case = arg0.toLowerCase()
                    if(arg0_lower_case == 'escape' || arg0 == ""){
                    }
                    else if( arg0_lower_case =="backspace" || arg0_lower_case == "delete"){
                        dbusKeyBinding.ModifyShortcut(currentShortcutId, "")
                    }
                    else {
                        var result = dbusKeyBinding.CheckShortcutConflict(arg0)
                        print("Release:", arg0, result)
                        switch(result[0]){
                            case "Invalid":
                                invalidInfoArea.showInvalidInfo(arg0);
                                break;
                            case "Conflict":
                                if (result[1][0]==currentShortcutId) return
                                canShowWarning = true
                                conflictInfoArea.showConfictArea(arg0, result[1]);
                                break;
                            case "Valid":
                                dbusKeyBinding.ModifyShortcut(currentShortcutId, arg0);
                                break;
                            default:
                                print("==> [ERROR]", arg0)
                        }
                    }
                    tipInfoText.keyPressed = false
                }
            }
            onKeyPressEvent: {
                if (currentShortcutId == shortcutId && grabFlag){
                    var arg0_lower_case = arg0.toLowerCase()
                    if(arg0_lower_case == 'escape' || arg0_lower_case == "" || arg0 == "backspace"){
                    }
                    else {
                        if(!tipInfoText.keyPressed){
                            tipInfoText.keyPressed = true
                        }
                        tipInfoText.keyPressInfo = mainObject.toHumanShortcutLabel(arg0)
                    }
                }
            }
        }

        MouseArea {
            height: parent.height
            width: grabKeyAreaWidth + 12
            anchors.right: parent.right
            hoverEnabled: true

            onEntered: {
                hovered = true
            }
            onExited: {
                hovered = false
            }

            onClicked: {
                if(stopSetKeyBinding) return;
                if(showDelete) return;
                grabFlag = true
                currentShortcutId = shortcutId
                dbusKeyBinding.GrabKbdAndMouse()
            }
        }
    }

    Rectangle{
        id: invalidInfoArea
        anchors.top: keyBindingArea.bottom
        width: parent.width
        height: 0
        color: dconstants.bgColor
        clip: true

        function showInvalidInfo(shortcutName){
            invalidInfo.text = dsTr('Shortcut \"%1\" is invalid, please retype new shortcut.').arg(mainObject.toHumanShortcutLabel(shortcutName))
            invalidInfoArea.height = conflictInfo.height + 15
            hideInvalidInfoArea.restart()
        }

        DssH2 {
            id: invalidInfo
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - 20
            wrapMode: Text.WordWrap
        }

        Timer{
            id: hideInvalidInfoArea
            repeat: false
            running: false
            interval: 1500
            onTriggered: {
                invalidInfoArea.height = 0
            }
        }
    }

    Item {
        id: conflictInfoArea
        anchors.top: keyBindingArea.bottom
        anchors.topMargin: 0 - arrowRectBackground.arrowHeight
        width: parent.width
        height: 0
        property int realHeight: childrenRect.height
        clip: true

        function showConfictArea(shortcutName, conflictIds){
            replaceButton.conflictKeyIds = conflictIds
            replaceButton.grabKeys = shortcutName
            confictText.text = mainObject.toHumanShortcutLabel(shortcutName)
            var info = dsTr('The shortcut you set ')
            for(var i in conflictIds){
                var keyBindingInfo = getKeyBindingInfo(conflictIds[i])
                info += dsTr('conflicts with the one used for \"%2\" in the \"%1\" category.').arg(categoryObjects[keyBindingInfo[3]]).arg(keyBindingInfo[1])
            }
            info += dsTr('Do you want to replace it?')

            conflictInfo.text = info
            conflictInfoArea.height = conflictInfoArea.realHeight
            stopSetKeyBinding = true
            inConfictDealing = true
        }

        ArrowRect{
            id: arrowRectBackground
            anchors.fill: parent
            fillStyle: dconstants.bgColor
            stroke: false
            radius: 0
            lineWidth: 0
        }

        Item{
            id: conflictInfoContent
            anchors.top: parent.top
            anchors.topMargin: arrowRectBackground.arrowHeight
            width: parent.width
            height: Math.max(conflictInfo.height, conflictWarningImg.height) + 16

            DssH2 {
                id: conflictInfo
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - conflictWarningImg.width - 20
                wrapMode: Text.WordWrap
                text: ""
            }

            Image{
                id: conflictWarningImg
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                source: "images/ico_warning.png"
            }
        }

        Item{
            width: parent.width
            height: 38
            anchors.top: conflictInfoContent.bottom

            Row {
                width: childrenRect.width
                height: childrenRect.height
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                DTextButton {
                    id: replaceButton
                    text: dsTr("Replace")
                    property var conflictKeyIds: new Array()
                    property string grabKeys: ""

                    onClicked: {
                        conflictInfoArea.height = 0
                        stopSetKeyBinding = false
                        inConfictDealing = false
                        for(var i in conflictKeyIds){
                            dbusKeyBinding.ModifyShortcut(conflictKeyIds[i], "")
                        }
                        dbusKeyBinding.ModifyShortcut(currentShortcutId, grabKeys)
                    }
                }

                DTextButton {
                    text: dsTr("Cancel")
                    onClicked: {
                        conflictInfoArea.height = 0
                        stopSetKeyBinding = false
                        inConfictDealing = false
                    }
                }
            }
        }
    }
}

