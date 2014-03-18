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
    property int grabKeyAreaWidth: field.width + 6

    property var info
    property int shortcutId: info[0]
    property string displayName: info[1]
    property string shortcutName: windowView.toHumanShortcutLabel(info[2])

    property bool grabFlag: false
    property bool showDelete: false
    property int contentLeftMargin: 22
    property bool hovered: false
    property bool inConfictDealing: false

    Behavior on height {
        PropertyAnimation { duration: 100 }
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
                bindManagerId.DeleteCustomBind(shortcutId)
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
            text: shortcutName? shortcutName : "Disable"
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
            id: field
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 11
            color: dconstants.fgDarkColor
            text: dsTr("Please input new shortcuts")
            visible: grabFlag
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
            target: grabManagerId
            onKeyReleaseEvent: {
                if (currentShortcutId == shortcutId){
                    grabFlag = false
                    if( arg0 == 'escape' | !arg0 ){
                    }
                    else if( arg0=="backspace" ){
                        bindManagerId.ChangeShortcut(currentShortcutId, "")
                    }
                    else {
                        var result = bindManagerId.CheckShortcut(arg0)
                        print("Release:", arg0, result)
                        switch(result[0]){
                            case "Invalid":
                                invalidInfoArea.showInvalidInfo(arg0);
                                break;
                            case "Conflict":
                                if (result[1][0]==currentShortcutId) return
                                conflictInfoArea.showConfictArea(arg0, result[1]);
                                break;
                            case "Valid":
                                bindManagerId.ChangeShortcut(currentShortcutId, arg0);
                                break;
                            default:
                                print("==> [ERROR]", arg0)
                        }
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
                grabManagerId.GrabKeyboard()
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
            invalidInfo.text = dsTr('无效的快捷键"%1"，请重新输入！').arg(windowView.toHumanShortcutLabel(shortcutName))
            invalidInfoArea.height = conflictInfo.height + 8
            hideInvalidInfoArea.restart()
        }

        DLabel {
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
            confictText.text = windowView.toHumanShortcutLabel(shortcutName)
            var info = dsTr('您设置的快捷键，')
            for(var i in conflictIds){
                var keyBindingInfo = getKeyBindingInfo(conflictIds[i])
                info += dsTr('与 "%1" 类别下的 "%2" 的快捷键冲突，').arg(categoryObjects[keyBindingInfo[3]]).arg(keyBindingInfo[1])
            }
            info += dsTr('是否进行替换？')

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

            DLabel {
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
                            bindManagerId.ChangeShortcut(conflictKeyIds[i], "")
                        }
                        bindManagerId.ChangeShortcut(currentShortcutId, grabKeys)
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

