import QtQuick 2.1

RotatingImage {
    MouseArea {
        anchors.fill: parent
        
        onClicked: {
            rotating = true
        }
    }
}