import QtQuick 2.1

Item {
    id: closeButton
    anchors.top: parent.top
    anchors.right: parent.right
    width: closeImage.width
    height: closeImage.height
    
    signal clicked
    
    Rectangle {
        id: closeBackground
        anchors.fill: parent
        anchors.topMargin: 3
        anchors.rightMargin: 3
        anchors.bottomMargin: 1
        anchors.leftMargin: 1
        color: Qt.rgba(0, 0, 0, 0)
    }
    
    Image {
        id: closeImage
        source: "images/window_close.png"
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            closeBackground.color = Qt.rgba(1, 1, 1, 0.3)
        }
        
        onExited: {
            closeBackground.color = Qt.rgba(1, 1, 1, 0)
        }
        
        onClicked: {
            closeButton.clicked()
        }
    }
}
