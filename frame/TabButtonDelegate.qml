import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: trayIconButton
    width: ListView.view.width
    height: 32
    smooth: true
    color: Qt.rgba(1, 0, 0, 0)
    
    property int tabIndex: 0
    property string trayIconId: iconId
    property string iconPathHeader: "trayicons/" + iconId
    property string extStr: iconId == "system_info" ? ".png": ".svg"
    
    property bool hover: false

    onHoverChanged: {
        if (hover){
            root.trayIconHoverHandler(trayIconId, index)
        }
        else {
            trayIconTip.visible = false
        }
    }

    ImageCheckButton {
        color: defaultBackgroundColor
        height: 32
        width: 32
        anchors.fill: parent
        inactivatedNomralImage: iconPathHeader + '_normal' + extStr
        inactivatedHoverImage: iconPathHeader + '_hover' + extStr
        inactivatedPressImage: inactivatedHoverImage

        activatedNomralImage: iconPathHeader + '_press' + extStr
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
