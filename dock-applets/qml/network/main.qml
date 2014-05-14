import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets/"

DockApplet{
    title: "DSS"
    appid: "AppletNetwork"
    icon: iconPath

    property var dconstants: DConstants {}
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var activeDevice: getActiveDevice()

    property url iconPath: {
        if(activeDevice){
            var accessPoints = unmarshalJSON(dbusNetwork.GetAccessPoints(activeDevice.Path))
            for(var i in accessPoints){
                var apObj = accessPoints[i]
                if(apObj.Path == activeDevice.ActiveAp){
                    var power = apObj.Strength
                    var step = 100
                    if(power <= 5){
                        step = 0
                    }
                    else if(power <= 25){
                        step = 25
                    }
                    else if(power <= 50){
                        step = 50
                    }
                    else if(power <= 75){
                        step = 75
                    }
                    else if(power <= 100){
                        step = 100
                    }
                    return getIconUrl("network/wifi-%1.png".arg(step))
                }
            }
        }

        return getIconUrl("network/wifi-0.png")
    }

    function getActiveDevice(){
        for(var i in wirelessDevices){
            var info = wirelessDevices[i]
            if(info.ActiveAp != "/" && info.State == 100){
                return info
            }
        }
        return null
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

    menu: Menu {
        Component.onCompleted: {
            addItem("_Run", showNetwork);
            addItem("_Undock", hideNetwork);
        }
    }

    window: DockQuickWindow {
        id: root
        width: 224
        height: content.height
        color: "transparent"

        Column {
            id: content
            width: parent.width

            Repeater {
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
