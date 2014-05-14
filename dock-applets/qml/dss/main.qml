import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0
import "../widgets/"

DockApplet{
    title: "DSS"
    appid: "AppletDss"
    icon: "preferences-system"

    // display
    property var dbusDisplay: Display{}
    property string monitorName: dbusDisplay.primary

    // vpn
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var vpnConnections: nmConnections["vpn"]

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

    onActivate:{
        showDss()
    }

    function showDss(){
        dbusControlCenter.ShowModule("home")
    }

    function hideDss(){
        set_hide_applet("dss")
    }

    menu: Menu{
        Component.onCompleted: {
            addItem("_Run", showDss);
            addItem("_Undock", hideDss);
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

                Item {
                    width: parent.width
                    height: 40

                    Image{
                        source: "images/light.png"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    WhiteSlider{
                        id: brightnessSlider
                        width: parent.width - 40
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        minimumValue: 0
                        maximumValue: 1.0

                        onValueChanged: {
                            if(pressed){
                                dbusDisplay.SetBrightness(monitorName, value)
                            }
                        }

                        Connections{
                            target: dbusDisplay
                            onBrightnessChanged: {
                                if(!brightnessSlider.pressed){
                                    brightnessSlider.value = dbusDisplay.brightness[monitorName]
                                }
                            }
                        }

                        Timer{
                            running: true
                            interval: 200
                            onTriggered: {
                                brightnessSlider.value = dbusDisplay.brightness[monitorName]
                            }
                        }

                    }
                }
            }

        }

    }
}
