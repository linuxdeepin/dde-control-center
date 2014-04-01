import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0
import Deepin.Window 1.0
import DGui 1.0

DWindowFrame{
    id: popupWindow
    z: 99

    property int innerWidth: 50
    property real posX: 0
    property real posY: 0

    x: posX - 25
    y: posY - 10

    width: innerWidth < 100 ? 100 : innerWidth + 6
    height: completeView.contentHeight + 6
    visible: false

    PopupItem {
        anchors.fill: parent
        windowObject: windowView
        rootWindowObject: rootWindow
        parentObject: popupWindow
    }

    Item {
        anchors.centerIn: parent
        width: parent.width - 6
        height: parent.height - 6

        ListView {
            id: completeView
            anchors.fill: parent
            model: 5
            delegate: MenuItem {}
            clip: true
            interactive: false
        }
    }

}
