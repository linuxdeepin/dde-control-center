import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: linkButtonWidget
    width: childrenRect.width
    height: childrenRect.height

    property alias text: label.text
    property color hoverColor: "#00bdff"
    signal clicked

    color: Qt.rgba(0, 0, 0, 0)

    DLabel {
        id: label
    }

    Rectangle {
        id: underLine
        height: 1
        width: label.width
        anchors.top: label.bottom
        color: hoverColor
        opacity: 0
    }

    states: [
        State {
            name: "hovered"
            PropertyChanges { target: label; color: hoverColor }
            PropertyChanges { target: underLine; opacity: 1 }
        },
        State {
            name: "pressed"
            PropertyChanges { target: label; color: hoverColor }
            PropertyChanges { target: underLine; opacity: 1 }
            //PropertyChanges { target: linkButtonWidget; color: Qt.rgba(0, 0, 0, 0.1) }
        }
    ]

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            parent.state = "hovered"
            mouseArea.cursorShape = Qt.PointingHandCursor
        }
        onExited: {
            parent.state = ""
            mouseArea.cursorShape = Qt.ArrowCursor
        }
        onPressed: {
            parent.state = "pressed"
            mouseArea.cursorShape = Qt.PointingHandCursor
        }
        onReleased: { 
            if(mouseArea.containsMouse){
                parent.state = "hovered"
                mouseArea.cursorShape = Qt.PointingHandCursor
            }
            else {
                parent.state = ""
                mouseArea.cursorShape = Qt.ArrowCursor
            }
        }
        onClicked: parent.clicked() 
    }
}
