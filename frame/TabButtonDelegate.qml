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
    
    property bool hover: false

    ImageCheckButton {
        color: defaultBackgroundColor
        anchors.fill: parent
        inactivatedNomralImage: iconPathHeader + '_normal.png'
        inactivatedHoverImage: iconPathHeader + '_hover.png'
        inactivatedPressImage: inactivatedHoverImage

        activatedNomralImage: iconPathHeader + '_press.png'
        activatedHoverImage: activatedNomralImage
        activatedPressImage: activatedNomralImage
        
        selected: trayIconButton.ListView.isCurrentItem ? true : false

        onClicked: {
            trayIconButton.ListView.view.currentIndex = index
        }

        onHoverChanged: {
            trayIconButton.hover = hover
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 1
        color: Qt.rgba(255, 255, 255, 0.1)
        visible: false
    }
}
