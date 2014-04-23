import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets/"

DockApplet{
    title: "DSS"
    appid: "AppletNetwork"
    icon: iconPath
    property url iconPath: "images/icon.png"
    property var dconstants: DConstants {}

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

    property var activeConnections: dbusNetwork.activeConnections

    function getWiredDeviceList(){
        var r = new Array()
        var devices = dbusNetwork.wiredDevices
        for(var i in devices){
            r.push(devices[i][0])
        }
        return r
    }
    property var wiredDeviceList: getWiredDeviceList()

    window: DockQuickWindow {
        id: root
        width: 280
        height: content.height
        color: Qt.rgba(0, 0, 0, 0.85)

        //Component.onCompleted: show()

        Column {
            id: content
            width: parent.width

            Repeater{
                id: wirelessNetworkRepeater
                model: wirelessDeviceList
                delegate: Loader {
                    id: wirelessNetworkArea
                    width: parent.width
                    height: childrenRect.height
                    source: "WirelessConnection.qml"
                }
            }
        }
    }
}
