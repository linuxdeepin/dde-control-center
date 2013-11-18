import QtQuick 2.1

Item {
    id: powerModuleTopItem
    Component.onCompleted: {
    }

    Rectangle {
        color: defaultBackgroundColor
        anchors.fill: parent
        Text {
            text: "Power Test"
            color: "white"
        }
    }
}
