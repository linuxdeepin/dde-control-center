import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    anchors.fill: parent
    clip: true

    function emitWarning(){
        //parent.tipNumber += 1
        //parent.showWarning = true
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
    }
}
