import QtQuick 2.1

Item {
    id: bar
    width: 100
    height: 8

    property real percentage: 0.5

    Rectangle{
        width: bar.width
        height: bar.height

        radius: height / 2
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 1.0; color: "#303132" }
        }
        anchors.fill: parent

        Rectangle {
            width: parent.width * bar.percentage
            height: parent.height
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            radius: height / 2
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0, 104/255, 170/255, 1.0) }
                GradientStop { position: 1.0; color: Qt.rgba(91/255, 164/255, 211/255, 1.0) }
            }
        }
    }
}
