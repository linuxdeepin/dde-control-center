import QtQuick 2.1
import QtGraphicalEffects 1.0

Item {
    id: root
    
    width: 55
    height: 24
    
    property bool checked: false
    state: checked ? "on" : "off"

    signal clicked

    transitions: Transition {
        NumberAnimation { properties: "x"; duration: 150; easing.type: Easing.InOutQuad }
    }
    
    Rectangle {
        id: mask

        width: 55
        height: 24
        color: "transparent"

        Rectangle {
            width: 54
            height: 18
            radius: 9

            anchors.centerIn: parent
        }

        anchors.centerIn: parent
    }

    Rectangle {
        id: inner

        width: 55
        height: 24
        color: "transparent"

        Image {
            id: inner_image
            source: "images/switch_inner.svg"

            anchors.verticalCenter: parent.verticalCenter
        }
        anchors.verticalCenter: parent.verticalCenter
    }

    OpacityMask {
        anchors.fill: mask
        source: ShaderEffectSource { id: shader_effect_source; sourceItem: inner; hideSource: true }
        maskSource: mask
    }

    states:  [
        State {
            name: "on"
            PropertyChanges { target: inner_image; x: 0}
        },
        State {
            name: "off"
            PropertyChanges { target: inner_image; x: -34}
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            checked = !checked
            root.clicked()
        }
    }
}
