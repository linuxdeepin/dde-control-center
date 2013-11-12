import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: trayIconButton
    width: ListView.view.width
    height: 50
    smooth: true
    color: Qt.rgba(1, 0, 0, 0)
    
    property int tabIndex: 0
    
    signal pressed
    
    Image {
        anchors.fill: parent
        source: trayIconButton.ListView.isCurrentItem ? trayIconImage + "_press.png" : trayIconImage + "_normal.png"
        scale: 0.8333

        InteractiveArea {
            id: mouseArea
            anchors.fill: parent

            onClicked: {
                trayIconButton.ListView.view.currentIndex = index
            }
        }
    }
}
