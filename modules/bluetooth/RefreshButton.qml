import QtQuick 2.1

RotatingImage {
    id:refreshButton

    signal clicked()

    MouseArea {
        anchors.fill: parent

        onClicked: {
            refreshButton.clicked()
        }
    }
}
