import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: showPasswordLine
    property DBaseLine targetTextInput
    
    visible: targetTextInput.visible
    
    rightLoader.sourceComponent: Row{
        width: valueWidth
        spacing: 6

        DSwitchButton{
            id: showPasswordButton
            checked: false
            onCheckedChanged: {
                targetTextInput.echoMode = checked ? TextInput.Normal : TextInput.Password
            }
        }

        DssH3{
            text: dsTr("Show password")
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
