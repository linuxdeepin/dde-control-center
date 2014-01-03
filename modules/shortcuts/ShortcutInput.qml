import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

FocusScope {
    id: keyBindItem
    width: parent.width
    height: 30

    property var info

    property var dconstants: DConstants {}
    property int grabKeyAreaWidth: 150

    property int shortcutId: info[0]
    property string displayName: info[1]
    property string shortcutName: windowView.toHumanShortcutLabel(info[2])

    property bool grabFlag: false

    DssH3 {
        text: displayName
        anchors.left: parent.left
        anchors.leftMargin: dconstants.leftMargin
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        width: grabKeyAreaWidth
        height: parent.height - 6
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        radius: 4
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
        anchors.fill: parent
        anchors.rightMargin: 15
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
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
                if( arg0 == 'Escape' | !arg0 ){
                }
                else if( arg0=="BackSpace" ){
                    //print("disable")
                    bindManagerId.ChangeShortcut(currentShortcutId, "")
                }
                else {
                    bindManagerId.ChangeShortcut(currentShortcutId, arg0)
                    print(bindManagerId.systemList)
                    print("------------")
                    print(keyBindings)
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            grabFlag = true
            currentShortcutId = shortcutId
            grabManagerId.GrabKeyboard()
        }
    }

}

