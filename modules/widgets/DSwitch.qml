import QtQuick 2.1

Image {
    id: switch_button
    state: "on"
    clip: true
    source: "images/switch_frame.svg"
    anchors.centerIn: parent

    transitions: Transition {
        NumberAnimation { properties: "x"; duration: 150; easing.type: Easing.InOutQuad }
    }

    Image {
        id: inner
        z: parent.z - 1
        source: "images/switch_inner.svg"

        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            switch_button.state = switch_button.state == "on" ? "off" : "on"
        }
    }

    states:  [
        State {
            name: "on"
            PropertyChanges { target: inner; x: 0}
        },
        State {
            name: "off"
            PropertyChanges { target: inner; x: -35}
        }
    ]
}
