import QtQuick 2.1
import Deepin.Widgets 1.0

DLabel {
    id: label
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    color: "#b4b4b4"
    font.pixelSize: 12

    signal clicked

    states: [
        State {
            name: "hovered"
            PropertyChanges { target: label; color: "#0090ff" }
            PropertyChanges { target: mouseArea; cursorShape: Qt.PointingHandCursor }
        },
        State {
            name: "pressed"
            PropertyChanges { target: label; color: "#006ec3" }
            PropertyChanges { target: mouseArea; cursorShape: Qt.PointingHandCursor }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "hovered"
            ColorAnimation { target: label; duration: 100}
        },
        Transition {
            from: "hovered"
            to: ""
            ColorAnimation { target: label; duration: 100}
        }
    ]

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        width: parent.width
        height: parent.height

        onEntered: label.state = "hovered"
        onExited: label.state = ""
        onPressed: label.state = "pressed"
        onReleased: containsMouse ? label.state = "hovered" : label.state = ""
        onClicked: label.clicked()
    }
}
