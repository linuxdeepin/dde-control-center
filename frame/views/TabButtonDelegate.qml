import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: trayIconButton
    smooth: true
    
    property int tabIndex: 0
    property string rightboxId: moduleId
    property string iconPathHeader: "trayicons/" + moduleId
    property string extStr: moduleId == "system_info" ? ".png": ".svg"
    
    property bool hover: false

    states: [
        State {
            name: "selected"
        }
    ]

    onHoverChanged: {
        trayIconTip.isHover = hover
        if (hover){
            root.trayIconHoverHandler(moduleId, index)
        }
        else {
            trayIconTip.timeOutHide.restart()
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
}
