import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    width: 300
    height: 300
    
    property variant constants: DConstants {}

    Item {
        width: 160
        height: 22

        Rectangle {
            id: text_edit_box

            width: 160
            height: 21
            color: "#1A1B1B"
            radius: 3
            border.color: "black"

            TextInput {
                color: constants.fgColor
                verticalAlignment: TextEdit.AlignVCenter
                font.pixelSize: 12
                echoMode: TextInput.Password
                
                anchors.fill: parent
                anchors.leftMargin: 3
                anchors.rightMargin: 3
            }
        }

        DropShadow {
            anchors.fill: text_edit_box
            radius: 1
            samples: 10
            horizontalOffset: 0
            verticalOffset: 1
            color: Qt.rgba(1, 1, 1, 0.15)
            source: text_edit_box
        }

        InnerShadow {
            anchors.fill: text_edit_box
            radius: 3
            samples: 10
            horizontalOffset: 0
            verticalOffset: radius
            color: "black"
            source: text_edit_box
        }

        anchors.centerIn: parent
    }
}
