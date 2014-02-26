import QtQuick 2.1

Item {
    signal selected

    state: "normal"
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: theme_thumbnail
                border.color: "white"
            }
            PropertyChanges {
                target: theme_name_box
                color: "transparent"
            }
            PropertyChanges {
                target: theme_name
                color: "#fff"
            }
        },
        State {
            name: "selected"
            PropertyChanges {
                target: theme_thumbnail
                border.color: "lightblue"
            }
            PropertyChanges {
                target: theme_name_box
                color: "#0D0D0D"
            }
            PropertyChanges {
                target: theme_name
                color: "#009EFF"
            }
        }
    ]

    Rectangle {
        id: theme_thumbnail
        width: parent.width
        height: 50
        color: "#092255"
        border.width: 1
    }

    Rectangle {
        id: theme_name_box
        radius: 3
        width: parent.width
        height: theme_name.implicitHeight + 6

        Text {
            id: theme_name
            text: "Theme" + index
            font.pixelSize: 12
            anchors.centerIn: parent
        }

        anchors.top: theme_thumbnail.bottom
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (parent.state == "normal") {
                parent.selected()
            } 
        }
    }
}