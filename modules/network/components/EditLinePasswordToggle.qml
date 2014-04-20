import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: showPasswordLine
    property DBaseLine targetTextInput
    
    rightLoader.sourceComponent: Row{
        width: valueWidth
        spacing: 6
        property bool showPassword: showPasswordButton.checked

        DSwitchButton{
            id: showPasswordButton
            onCheckedChanged: {
                targetTextInput.echoMode = showPassword ? TextInput.Normal : TextInput.Password
            }
            Component.onCompleted: {
                checked = true
            }
        }

        DssH3{
            text: dsTr("Show password")
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
