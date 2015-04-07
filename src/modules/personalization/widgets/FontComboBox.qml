import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import Deepin.Widgets 1.0

Item {
    id: combobox
    width: Math.max(minMiddleWidth, parent.width)
    height: background.height

    property bool hovered: false
    property bool pressed: false

    property alias comboboxText: currentLabel.text
    property alias menu: menu

    property var parentWindow
    property var labels
    property int initIndex: 0

    signal clicked
    signal menuSelect(int index)

    Component.onCompleted: {
        if(initIndex != -1){
            comboboxText = menu.labels[initIndex]
        }
    }

    Timer {
        id:refreshTimer
        interval: 1000
        repeat: true
        onTriggered: {
            if (!menu.visible)
                rootWindow.update()
        }
    }

    Timer {
        id:menuRefreshTimer
        interval: 50
        repeat: false
        onTriggered: {
            menu.update()
        }
    }

    FontComboBoxMenu {
        id: menu
        parentWindow: combobox.parentWindow
        labels: combobox.labels
        currentTextValue: comboboxText
        onMenuSelect: {
            combobox.menuSelect(index)
            comboboxText = labels[index]
        }
        onMenuEnter: menuRefreshTimer.start()
        onVisibleChanged: {
            refreshTimer.start()
        }
    }

    function showMenu(x, y, w) {
        menu.x = x - menu.frameEdge + 1
        menu.y = y - menu.frameEdge - combobox.height
        menu.width = w + menu.frameEdge * 2 -2
        menu.showMenu()
        menuRefreshTimer.start()
    }

    onClicked: {
        var pos = mapToItem(null, 0, 0)
        var x = parentWindow.x + pos.x
        var y = parentWindow.y + pos.y + height
        var w = width
        showMenu(x, y, w)
    }

    QtObject {
        id: buttonImage
        property string status: "normal"
        property string header: "images/button_left_%1.png".arg(status)
        property string middle: "images/button_center_%1.png".arg(status)
        property string tail: "images/button_right_%1.png".arg(status)
    }

    property int minMiddleWidth: buttonHeader.width + downArrow.width + buttonTail.width

    Row {
        id: background
        height: buttonHeader.height
        width: parent.width

        Image{
            id: buttonHeader
            source: buttonImage.header
        }

        Image {
            id: buttonMiddle
            source: buttonImage.middle
            width: parent.width - buttonHeader.width - buttonTail.width
        }

        Image{
            id: buttonTail
            source: buttonImage.tail
        }
    }

    Rectangle {
        id: content
        width: buttonMiddle.width
        height: background.height
        anchors.left: parent.left
        anchors.leftMargin: buttonHeader.width
        anchors.verticalCenter: parent.verticalCenter
        color: Qt.rgba(1, 0, 0, 0)

        DLabel {
            id: currentLabel
            focus: true
            property QtObject dconstants: DConstants {}
            color: dconstants.fgColor
            font.pixelSize: 12
            font.family: comboboxText
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - downArrow.width
            elide: Text.ElideRight
        }

        Image {
            id: downArrow
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            source: hovered ? "images/arrow_down_hover.png" : "images/arrow_down_normal.png"
        }

    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            parent.hovered = true
        }

        onExited: {
            parent.hovered = false
        }

        onPressed: {
            parent.pressed = true
            buttonImage.status = "press"
        }
        onReleased: {
            parent.pressed = false
            parent.hovered = containsMouse
            buttonImage.status = "normal"
        }

        onClicked: {
            combobox.clicked()
        }
    }

}
