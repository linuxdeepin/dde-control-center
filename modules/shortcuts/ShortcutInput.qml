import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

FocusScope {
    id: keyBindItem
    width: parent.width
    height: 30

    property var info
    property var warning

    property var dconstants: DConstants {}
    property int grabKeyAreaWidth: 150

    property int shortcutId: info[0]
    property string displayName: info[1]
    property string shortcutName: windowView.toHumanShortcutLabel(info[2])

    property bool grabFlag: false

    Row {
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: warning ? 2 : dconstants.leftMargin + 4
        spacing: 4

        Item {
            height: parent.height
            width: childrenRect.width
            visible: shortcutId >= 10000

            DOpacityImageButton{
                id: deleteButton
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 2
                source: "images/clear.png"
            }
        }

        Image{
            source: warning ? "images/" + warning + "_key.png" :""
            visible: source ? true : false
            anchors.verticalCenter: parent.verticalCenter
        }

        DssH3 {
            text: displayName
            anchors.verticalCenter: parent.verticalCenter
        }
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
                if( arg0 == 'escape' | !arg0 ){
                }
                else if( arg0=="backspace" ){
                    bindManagerId.ChangeShortcut(currentShortcutId, "")
                }
                else {
                    bindManagerId.ChangeShortcut(currentShortcutId, arg0)
                }
            }
        }
    }

    MouseArea {
        height: parent.height
        width: grabKeyAreaWidth + 12
        anchors.right: parent.right
        onClicked: {
            grabFlag = true
            currentShortcutId = shortcutId
            grabManagerId.GrabKeyboard()
        }
    }

}

