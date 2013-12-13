import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: round_image

    property int roundRadius: 50
    property url imageSource: ""
    property int borderWidth: 3
    property color borderColor: "white"
    property int glowRadius: 9
    property color glowColor: "lightsteelblue"

    property bool checkSupport: false

    width: (roundRadius + borderWidth) * 2
    height: (roundRadius + borderWidth) * 2
    radius: roundRadius + borderWidth
    color: borderColor
    state: "normal"
    
    signal clicked
    
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: glow
                visible: false
            }
        },
        State {
            name: "checked"
            PropertyChanges {
                target: glow
                visible: true
            }
        }
    ]

    DRoundItem {
        id: round_item

        width: radius * 2
        height: radius * 2
        radius: parent.roundRadius

        Image {
            source: round_image.imageSource
        }

        anchors.centerIn: parent
    }

    Glow {
        id: glow

        anchors.fill: round_item
        radius: round_image.glowRadius
        samples: 10
        color: round_image.glowColor
        source: round_item
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            parent.state = parent.checkSupport ? (parent.state == "normal" ? "checked" : "normal") : "normal"
            parent.clicked()
        }
    }
}
