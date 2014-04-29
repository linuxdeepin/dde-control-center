import QtQuick 2.1

Image {
    id: actionButton

    source: "images/preview.png"

    opacity: 1.0
    property alias mouseArea: mouseArea

    signal clicked
    
    states: State {
        name: "hovered"
        PropertyChanges { target: actionButton; opacity: 1.0 }
    }
    
    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 350 }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: actionButton
        hoverEnabled: true
        
        onEntered: {
            actionButton.state = "hovered"
            mouseArea.cursorShape = Qt.PointingHandCursor
            toolTip.showTip(dsTr("Preview theme"))
        }
        
        onExited: {
            actionButton.state = ""
            mouseArea.cursorShape = Qt.ArrowCursor
            toolTip.hideTip()
        }
        
        onReleased: { 
            actionButton.state = mouseArea.containsMouse ? "hovered" : ""
        }
        onClicked: {
            parent.clicked()
        }
    }
}

