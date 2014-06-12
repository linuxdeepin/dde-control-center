import QtQuick 2.0
import Deepin.Widgets 1.0

Row{
    spacing: 4

    DTextInput {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - portLabel.width - portText.width - parent.spacing * 2 - 2
    }
    DssH2 {
        id: portLabel
        text: dsTr("Port")
        anchors.verticalCenter: parent.verticalCenter
    }
    DTextInput {
        id: portText
        anchors.verticalCenter: parent.verticalCenter
        width: 40
        textInput.validator: IntValidator{}
    }
}
