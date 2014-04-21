import QtQuick 2.0
import Deepin.Widgets 1.0

DImageCheckButton{
    id: button
    anchors.verticalCenter: parent.verticalCenter
    inactivatedNomralImage: "images/wire_off.png"
    inactivatedHoverImage: inactivatedNomralImage
    inactivatedPressImage: inactivatedNomralImage

    activatedNomralImage: "images/wire_on.png"
    activatedHoverImage: activatedNomralImage
    activatedPressImage: activatedNomralImage

    onClicked: {
        dbusNetwork.wiredEnabled = active
    }

    Connections{
        target: dbusNetwork
        onWiredEnabledChanged:{
            if(!button.pressed){
                button.active = dbusNetwork.wiredEnabled
            }
        }
    }

    Timer{
        running: true
        interval: 100
        onTriggered: {
            parent.active = dbusNetwork.wiredEnabled
        }
    }
}
