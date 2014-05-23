import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: weekTitleItem
    width: itemWidth
    height: weekTitleHeight

    property color borderColor: Qt.rgba(1, 1, 1, 0.2)

    Item {
        id: content
        width: itemWidth - 1
        height: weekTitleHeight - 1

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(57/255, 64/255, 32/255, 0.5)
            visible: weekTitleItem.GridView.view.currentIndex == index
        }

        DssH2 {
            id: weekTitleLabel
            anchors.centerIn: parent
            text: dayText
        }
    }

    Rectangle {
        id: rightBorder
        anchors.left: content.right
        width: 1
        height: parent.height
        color: borderColor
        visible: (index + 1) % 7 != 0
    }

    Rectangle {
        id: bottomBorder
        anchors.top: content.bottom
        width: parent.width
        height: 1
        color: borderColor
    }
}
