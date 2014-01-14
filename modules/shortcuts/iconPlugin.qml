import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.BindManager 1.0

Item {
    anchors.fill: parent
    clip: true

    property var bindManagerId: BindManager {}

    Binding {
        target: parent
        property: "showWarning"
        value: bindManagerId.conflictInvalid.length !=0 ? true : false
    }

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
