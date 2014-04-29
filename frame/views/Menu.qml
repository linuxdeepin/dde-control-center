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

    property alias currentIndex: completeView.currentIndex

    x: posX - 28
    y: posY - 12

    width: innerWidth + 24
    height: completeViewBox.height + 32
    visible: false

    property var labels: []
    property Item requestMenuItem: popupWindow

    function menuSelect(i){}

    PopupItem {
        anchors.fill: parent
        windowObject: windowView
        rootWindowObject: rootWindow
        parentObject: popupWindow
    }

    Item {
        id: completeViewBox
        anchors.centerIn: parent
        width: parent.width - 6
        height: childrenRect.height

        ListView {
            id: completeView
            width: parent.width
            height: childrenRect.height
            model: labels
            delegate: MenuItem {
                text: labels[index]
                onSelectAction:{
                    popupWindow.visible = false
                    if(popupWindow.requestMenuItem){
                        popupWindow.requestMenuItem.menuSelect(index)
                    }
                }
            }
            clip: true
            interactive: false
        }
    }

}
