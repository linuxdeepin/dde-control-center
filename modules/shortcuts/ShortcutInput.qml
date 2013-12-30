import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

FocusScope {
    id: keyBindItem
    width: parent.width
    height: 30

    property var dconstants: DConstants {}
    property int grabKeyAreaWidth: 150

    //property string shortcutName: ""
    property string keybindLabel: ""
    property string oldShortcut: ""

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
        color: field.grabFlag ? "#101010" : "transparent"
    }

    Text {
        id: field
        anchors.fill: parent
        anchors.rightMargin: 15
        focus: true
        horizontalAlignment: TextInput.AlignRight
        verticalAlignment: TextInput.AlignVCenter
        font.pixelSize: 11
        color: activeFocus ? dconstants.fgDarkColor : dconstants.fgColor
        text: shortcutName? shortcutName : "Disable"

        property bool grabFlag: false
        property int myShortcutId: shortcutId

        Connections {
            target: grabManagerId
            onGrabKeyEvent: {
                if (currentShortcutId == field.myShortcutId){
                    field.grabFlag = false
                    print(arg0)
                    if( arg0 == 'Escape' | !arg0 ){
                        field.text = oldShortcut
                    }
                    else if( arg0=="BackSpace" ){
                        field.text = dsTr("Disable")
                    }
                    else {
                        field.text = windowView.toHumanShortcutLabel(arg0)
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                oldShortcut = field.text
                field.text = dsTr("Please input new shortcuts")
                field.grabFlag = true
                currentShortcutId = field.myShortcutId
                grabManagerId.GrabKeyboard()
            }
        }
    }
}

