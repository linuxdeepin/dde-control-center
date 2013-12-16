import QtQuick 2.1

Rectangle {
    anchors.left: parent.left
    width: parent.width
    height: 28
    color: "#1a1b1b"

    property int timezoneValue: value
    property string timezoneText: textD

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 13
        color: parent.ListView.view.currentIndex == index ? Qt.rgba(0, 144/255, 1, 1.0) :dconstants.fgColor
        text: textD
        Component.onCompleted: {
            //print(width)
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: { parent.ListView.view.currentIndex = index }
    }
}

