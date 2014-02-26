import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    id: keyBindingItem
    width: parent.width
    height: 30
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

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    function showConfictArea(info){
        conflictInfo.text = "快捷键冲突: "+ info +"，是否替换？"
        keyBindingItem.height = keyBindingArea.height + conflictInfoArea.height
    }

    function showInvalidInfo(){
        conflictInfo.text = "无效的快捷键，请重新输入"
        keyBindingItem.height = keyBindingArea.height + conflictInfo.height + 8
        hideConfictInfoArea.restart()
    }

    Timer{
        id: hideConfictInfoArea
        repeat: false
        running: false
        interval: 1500
        onTriggered: {
            keyBindingItem.height = keyBindingArea.height
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
            color: dconstants.fgColor
            text: shortcutName? shortcutName : "Disable"
            visible: !grabFlag
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

        Connections {
            target: grabManagerId
            onKeyReleaseEvent: {
                if (currentShortcutId == shortcutId){
                    grabFlag = false
                    print("Release:", arg0)
                    if( arg0 == 'escape' | !arg0 ){
                    }
                    else if( arg0=="backspace" ){
                        bindManagerId.ChangeShortcut(currentShortcutId, "")
                    }
                    else if( arg0=="a" ){
                        showInvalidInfo()
                    }
                    else {
                        showConfictArea(arg0)
                        //bindManagerId.ChangeShortcut(currentShortcutId, arg0)
                    }
                }
            }
        }

        MouseArea {
            height: parent.height
            width: grabKeyAreaWidth + 12
            anchors.right: parent.right
            onClicked: {
                if(!showDelete){
                    grabFlag = true
                    currentShortcutId = shortcutId
                    grabManagerId.GrabKeyboard()
                }
            }
        }
    }

    Rectangle {
        id: conflictInfoArea
        anchors.top: keyBindingArea.bottom
        width: parent.width
        height: childrenRect.height
        color: dconstants.bgColor

        DLabel {
            id: conflictInfo
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 4
            width: parent.width - 30
            wrapMode: Text.WordWrap
            text: ""
        }

        Item{
            width: parent.width
            height: 38
            anchors.top: conflictInfo.bottom

            Row {
                width: childrenRect.width
                height: childrenRect.height
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                DTextButton {
                    text: dsTr("Replace")
                    onClicked: {
                        keyBindingItem.height = keyBindingArea.height
                    }
                }

                DTextButton {
                    text: dsTr("Cancel")
                    onClicked: {
                        keyBindingItem.height = keyBindingArea.height
                    }
                }
            }
        }
    }
}

