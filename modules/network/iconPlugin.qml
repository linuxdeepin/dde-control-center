import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    anchors.fill: parent
    clip: true

    function emitWarning(){
        parent.tipNumber += 1
    }

    QtObject {
        id: wiredIcon
        property url normalImage: "img/network_wire_normal.svg"
        property url hoverImage: "img/network_wire_hover.svg"
        property url pressImage: hoverImage
    }

    //Timer {
        //interval: 1000
        //running: true
        //repeat: true
        //onTriggered: {
            //emitWarning()
        //}
    //}

    Component.onCompleted: {
        //parent.tipNumber = 97
        //parent.showWarning = true
        //parent.icon = wiredIcon
    }
}
