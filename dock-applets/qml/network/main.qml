import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets/"

DockApplet{
    title: "DSS"
    appid: "AppletNetwork"
    icon: iconPath
    property url iconPath: "images/network_on.png"
    property var dconstants: DConstants {}

    function marshalJSON(value){
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }
    
    function showNetwork(){
        dbusControlCenter.ShowModule("network")
    }

    function hideNetwork(){
        set_hide_applet("network")
    }

    onActivate: {
        showNetwork()
    }

    menu: Menu{
        Component.onCompleted: {
            addItem("_Run", showNetwork);
            addItem("_Undock", hideNetwork);
        }
    }

    property var nmActiveConnections: unmarshalJSON(dbusNetwork.activeConnections)
    function isActiveConnection(devPath, uuid) {
        for (var i in nmActiveConnections) {
            if (getIndexFromArray(devPath, nmActiveConnections[i].Devices) != -1 &&
            nmActiveConnections[i].Uuid == uuid) {
                return true
            }
        }
        return false
    }
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)

    window: DockQuickWindow {
        id: root
        width: 280
        height: content.height
        color: "transparent"

        Column {
            id: content
            width: parent.width

            Repeater{
                id: wirelessNetworkRepeater
                model: wirelessDevicesNumber
                delegate: Loader {
                    id: wirelessNetworkArea
                    width: parent.width
                    height: childrenRect.height
                    source: "WirelessNetwork.qml"
                }
            }
        }
    }
}
