import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0
import "../widgets/"

DockApplet{
    title: "DSS"
    appid: "AppletNetwork"
    icon: iconPath

    property var dconstants: DConstants {}

    // vpn
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var vpnConnections: nmConnections["vpn"]
    property var activeDevice: getActiveDevice()

    onVpnConnectionsChanged: {
        if(vpnConnections){
            print("VPN number:",vpnConnections.length)
        }
        else{
            print("No VPN Connections...")
        }
    }

    // bluetooth
    property var dbusBluetooth: Bluetooth {}
    property var adapters: dbusBluetooth.adapters ? unmarshalJSON(dbusBluetooth.adapters) : ""

    property int xEdgePadding: 0


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
        height: contentColumn.height + xEdgePadding * 2
        color: "transparent"

        Item {
            anchors.centerIn: parent
            width: parent.width - xEdgePadding * 2
            height: parent.height - xEdgePadding * 2

            Column {
                id: contentColumn
                width: parent.width
                spacing: 20

                Row {
                    id: buttonRow
                    spacing: 16
                    anchors.horizontalCenter: parent.horizontalCenter

                    CheckButton{
                        id: wiredCheckButton
                        onImage: "images/wire_on.png"
                        offImage: "images/wire_off.png"
                        visible: dbusNetwork.wiredDevices.length > 0

                        onClicked: {
                            dbusNetwork.wiredEnabled = active
                        }

                        Connections{
                            target: dbusNetwork
                            onWiredEnabledChanged:{
                                if(!wiredCheckButton.pressed){
                                    wiredCheckButton.active = dbusNetwork.wiredEnabled
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

                    CheckButton{
                        id: wirelessCheckButton
                        onImage: "images/wifi_on.png"
                        offImage: "images/wifi_off.png"
                        visible: dbusNetwork.wirelessDevices.length > 0

                        onClicked: {
                            dbusNetwork.wirelessEnabled = active
                        }

                        Connections{
                            target: dbusNetwork
                            onWirelessEnabledChanged:{
                                if(!wirelessCheckButton.pressed){
                                    wirelessCheckButton.active = dbusNetwork.wirelessEnabled
                                }
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                parent.active = dbusNetwork.wirelessEnabled
                            }
                        }
                    }

                    //CheckButton{
                        //onImage: "images/3g_on.png"
                        //offImage: "images/3g_off.png"
                    //}

                    CheckButton{
                        id: vpnButton
                        visible: vpnConnections ? vpnConnections.length > 0 : false
                        onImage: "images/vpn_on.png"
                        offImage: "images/vpn_off.png"
                        property var activeConnections: dbusNetwork.ActiveConnections
                        property bool vpnActive: getVpnActivated() != -1

                        onVpnActiveChanged: {
                            if(!vpnButton.pressed){
                                vpnButton.active = vpnActive
                            }
                        }

                        function getVpnActivated(){
                            for(var i in activeConnections){
                                if(activeConnections[i].Vpn){
                                    return i
                                }
                            }
                            return -1
                        }

                        function deactiveVpn(){
                            var index = getVpnActivated()
                            if(index != -1){
                                var uuid = activeConnections[i].Uuid
                                dbusNetwork.DeactivateConnection(uuid)
                            }
                        }

                        onClicked: {
                            deactiveVpn()
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                parent.active = parent.vpnActive
                            }
                        }
                    }

                    CheckButton{
                        id: bluetoothButton
                        visible: adapters.length > 0
                        onImage: "images/bluetooth_on.png"
                        offImage: "images/bluetooth_off.png"

                        onClicked: {
                            dbusBluetooth.powered = active
                        }

                        Connections{
                            target: dbusBluetooth
                            onPoweredChanged:{
                                if(!bluetoothButton.pressed){
                                    bluetoothButton.active = dbusBluetooth.powered
                                }
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                if(dbusBluetooth.powered)
                                    parent.active = dbusBluetooth.powered
                            }
                        }
                    }

                    CheckButton {
                        id: airplaneModeButton
                        onImage: "images/airplane_mode_on.png"
                        offImage: "images/airplane_mode_off.png"
                        property bool airplaneModeActive: getActive()

                        onAirplaneModeActiveChanged: {
                            if(!airplaneModeButton.pressed){
                                airplaneModeButton.active = airplaneModeButton.airplaneModeActive
                            }
                        }

                        function getActive(){
                            if(dbusNetwork.networkingEnabled || dbusBluetooth.powered){
                                return false
                            }
                            else{
                                return true
                            }
                        }

                        function setActive(){
                            dbusNetwork.networkingEnabled = true
                            dbusNetwork.wiredEnabled = true
                            dbusNetwork.wirelessEnabled = true
                            dbusBluetooth.powered = true
                        }

                        function setDeactive(){
                            dbusNetwork.wiredEnabled = false
                            dbusNetwork.wirelessEnabled = false
                            dbusNetwork.networkingEnabled = false
                            dbusBluetooth.powered = false
                        }

                        onClicked: {
                            if(active){
                                setDeactive()
                            }
                            else{
                                setActive()
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                parent.active = parent.airplaneModeActive
                            }
                        }

                    }
                }

            }

        }

    }

}
