import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle{
    id: buttonOuterBox
    width: content.width + xPadding * 2
    height: content.height + yPadding * 2
    border.width: hasBorder ? 1 : 0
    radius: 2

    property alias contentLoader: content

    signal clicked

    property int xPadding: 10
    property int yPadding: 2
    property color hoverBgColor: Qt.rgba(0, 0, 0, 0.2)
    property color hoverBorderColor: Qt.rgba(1, 1, 1, 0.2)
    property bool hasBorder: true

    property var dconstants: DConstants {}
    property bool hovered: false
    property bool pressed: false 

    color: {
        if(hovered || pressed){
            return hoverBgColor
        }
        else{
            return "transparent"
        }
    }

    border.color: {
        if(hovered || pressed){
            return hoverBorderColor
        }
        else{
            return "transparent"
        }
    }

    Loader{
        id: content
        anchors.centerIn: parent
        width: item.width
        height: item.height
        property bool pressed: parent.pressed
        property bool hovered: parent.hovered
    }

    MouseArea {
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
        }

        onReleased: {
            parent.pressed = false
            parent.hovered = containsMouse
        }
        onClicked: parent.clicked()
    }
}
