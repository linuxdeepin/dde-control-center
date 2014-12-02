import QtQuick 2.1

Item {
    width: 300
    height: 10

    property alias radius: bg.radius

    Rectangle {
        id: bg
        color: Qt.rgba(15/255, 15/255, 15/255, 1.0)
        width: parent.width
        height: parent.height - 1
        radius: height / 2

        Rectangle {
            width: parent.width - parent.height
            height: 1
            gradient: Gradient {
                GradientStop { position: 0.0; color: "black" }
                GradientStop { position: 1.0; color: "#303132" }
            }
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
        }
    }

    Rectangle {
        width: parent.width - parent.height
        height: 1
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
    }
}