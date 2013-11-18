import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: trayIconButton
    width: ListView.view.width
    height: 50
    smooth: true
    color: Qt.rgba(1, 0, 0, 0)
    
    property int tabIndex: 0
    property string trayIconId: iconId
    property string iconPathHeader: "trayicon_images/" + iconId
    
    signal pressed
    
    Image {
        anchors.fill: parent
        source: trayIconButton.ListView.isCurrentItem ? iconPathHeader + "_press.png" : iconPathHeader + "_normal.png"
        scale: 0.8333

        MouseArea {
            anchors.fill: parent

            onClicked: {
                trayIconButton.ListView.view.currentIndex = index
            }
        }
    }
}
