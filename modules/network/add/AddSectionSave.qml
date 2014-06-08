import QtQuick 2.1
import Deepin.Widgets 1.0

Row {
    height: 38
    spacing: 10
    anchors.right: parent.right
    anchors.rightMargin: 15

    DTextButton {
        text: dsTr("Cancel")
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            stackView.reset()
            connectionSession.Close()
        }
    }
    
    DTextButton {
        text: dsTr("Add")
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            checkKeysInPage()
            if (connectionSession.Save()) {
                stackView.reset()
            }
        }
    }
}
