import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle{
    id: buttonOuterBox
    width: label.width + xPadding * 2
    height: label.height + yPadding * 2
    border.width: hasBorder ? 1 : 0
    radius: 2

    signal clicked

    property alias text: label.text
    property int xPadding: 10
    property int yPadding: 2
    property color hoverBgColor: Qt.rgba(0, 0, 0, 0.2)
    property color hoverBorderColor: Qt.rgba(1, 1, 1, 0.2)
    property bool hasBorder: false

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

    DLabel {
        id: label
        anchors.centerIn: parent

        color: {
            if(parent.pressed){
                return dconstants.activeColor
            }
            else if (parent.hovered){
                return dconstants.hoverColor
            }
            else{
                return dconstants.fgColor
            }
        }
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
