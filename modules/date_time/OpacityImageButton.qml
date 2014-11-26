import QtQuick 2.1

Image {
    id: actionButton

    opacity: 0.5
    property alias mouseArea: mouseArea

    signal clicked
    
    states: State {
        name: "hovered"
        PropertyChanges { target: actionButton; opacity: 1.0 }
    }
    
    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 200 }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: actionButton
        hoverEnabled: true
        
        onEntered: {
            actionButton.state = "hovered"
        }
        
        onExited: {
            actionButton.state = ""
        }
        
        onReleased: { 
            actionButton.state = mouseArea.containsMouse ? "hovered" : ""
        }
        onClicked: {
            parent.clicked()
        }
    }
}
