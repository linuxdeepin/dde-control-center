import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: wrapper
    width: wrapper.ListView.view.width; height: 26
    Rectangle {
        color: wrapper.ListView.view.currentIndex == index ? "#141414" : "#232323"
        anchors.fill: parent
    }

    DssH2 {
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        text: "text " + index
        color: wrapper.ListView.view.currentIndex == index ? dconstants.activeColor : dconstants.fgColor
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onContainsMouseChanged: {
            if (containsMouse && wrapper.ListView.view.currentIndex != index) {
                wrapper.ListView.view.currentIndex = index
            }
        }
        onClicked: print(index)
    }
}
