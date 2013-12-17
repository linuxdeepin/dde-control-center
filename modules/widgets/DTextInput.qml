import QtQuick 2.1
import QtGraphicalEffects 1.0

Item {
    width: 160
    height: 22

    property alias echoMode: text_input.echoMode
    property variant constants: DConstants {}

    Rectangle {
        id: text_input_box

        width: parent.width
        height: 22
        clip: true
        color: "#1A1B1B"
        radius: 3
        border.color: "black"
    }

    DropShadow {
        anchors.fill: text_input_box
        radius: 1
        samples: 10
        horizontalOffset: 0
        verticalOffset: 1
        color: Qt.rgba(1, 1, 1, 0.15)
        source: text_input_box
    }

    InnerShadow {
        anchors.fill: text_input_box
        radius: 3
        samples: 10
        horizontalOffset: 0
        verticalOffset: radius
        color: "black"
        source: text_input_box
    }

    Item {
        clip: true
        anchors.fill: text_input_box
        
        TextInput {
            id: text_input

            color: constants.fgColor
            selectByMouse: true
            verticalAlignment: TextInput.AlignVCenter
            font.pixelSize: text_input.echoMode == TextInput.Password ? 18 : 12

            anchors.fill: parent
            anchors.leftMargin: 3
            anchors.rightMargin: 3
        }
    }
}
