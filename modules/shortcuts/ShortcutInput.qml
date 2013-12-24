import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets"

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
        color: field.activeFocus ? "#101010" : "transparent"
    }

    TextInput {
        id: field
        anchors.fill: parent
        anchors.rightMargin: 15
        readOnly: true
        cursorVisible: false
        focus: true
        horizontalAlignment: TextInput.AlignRight
        verticalAlignment: TextInput.AlignVCenter
        font.pixelSize: 11
        color: activeFocus ? dconstants.fgDarkColor : dconstants.fgColor
        text: shortcutName? shortcutName : "Disable"

        Keys.onPressed: {
            if(event.key == Qt.Key_Escape){
                field.focus = false
            }
            print(event.key, event.text)
            //if (event.modifiers){
                //print(event.modifiers)
            //}
        }

        Keys.onReleased: {
        }

        onActiveFocusChanged: {
            if (activeFocus){
                oldShortcut = text
                text = dsTr("Please input new shortcuts")
                //print(windowView.grabKeyboard(true))
            }
            else {
                text = oldShortcut
                //print(windowView.grabKeyboard(false))
            }
        }

    }
}

