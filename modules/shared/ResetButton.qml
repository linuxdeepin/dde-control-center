import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    width: successLabel.width
    height: childrenRect.height

    signal clicked

    property bool inSuccessState: false

    DssH2 {
        id: successLabel
        font.pixelSize: 12
        color: dconstants.activeColor
        text: dsTr("Reset the success")
        visible: opacity > 0
        opacity: inSuccessState ? 1 : 0
        NumberAnimation on opacity { duration: 150 }
    }

    DTextButton {
        anchors.right: parent.right
        text: dsTr("Reset")
        visible: opacity > 0
        opacity: inSuccessState ? 0 : 1
        NumberAnimation on opacity { duration: 150 }

        onClicked: {
            parent.clicked()
            parent.inSuccessState = true
            showButtonTimer.start()
        }
    }

    Timer{
        id: showButtonTimer
        interval: 1000
        onTriggered: {
            parent.inSuccessState = false
        }
    }

}
