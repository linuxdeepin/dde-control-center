import QtQuick 2.1
import QtGraphicalEffects 1.0

FocusScope {
    id: root
    width: 160
    height: 22
    state: "normal"

    property alias echoMode: text_input.echoMode
    property alias text: text_input.text
    property int textInputRightMargin: 0
    property variant constants: DConstants {}
    
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: text_input_box
                border.color: "black"
            }
        },
        State {
            name: "warning"
            PropertyChanges {
                target: text_input_box
                border.color: "red"
            }
        }
    ]
    
    Rectangle {
        id: text_input_box

        width: parent.width
        height: 22
        clip: true
        color: "#1A1B1B"
        radius: 3
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
        anchors.rightMargin: root.textInputRightMargin
        
        TextInput {
            id: text_input

            focus: true
            color: constants.fgColor
            selectByMouse: true
            verticalAlignment: TextInput.AlignVCenter
            font.pixelSize: text_input.echoMode == TextInput.Password ? 18 : 12
            
            anchors.fill: parent
            anchors.leftMargin: 3
            anchors.rightMargin: 3
        }
    }
    
    MouseArea {
        anchors.fill: parent
        onPressed: {
            mouse.accepted = false
            if (root.state == "warning") {
                root.state = "normal"
            }
        }
    }
}
