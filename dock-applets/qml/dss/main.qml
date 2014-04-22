import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import "../widgets/"

DockApplet{
    title: "DSS"
    appid: "AppletDss"
    icon: iconPath
    property url iconPath: "images/icon.png"

    // display
    property var dbusDisplay: Display{}
    property var monitorObject: Monitor{ path: dbusDisplay.monitors[0] }
    property var brightnessDict: monitorObject.brightness
    property string monitorName: Object.keys(brightnessDict)[0]

    // network
    property var dbusNetwork: NetworkManager{}

    property int xEdgePadding: 10

    onActivate:{
        showDss()
    }

    function showDss(){
        dbusControlCenter.ShowModule("home")
    }

    function hideDss(){
        set_hide_applet("dss")
    }

    window: DockQuickWindow {
        id: root
        width: buttonRow.width + xEdgePadding * 2
        height: contentColumn.height + xEdgePadding * 2
        color: Qt.rgba(0, 0, 0, 0.85)

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

                    Loader {
                        id: wiredButtonLoader
                        source: dbusNetwork.wiredDevices.length > 0 ? "WiredCheckButton.qml" : ""
                    }

                    Loader {
                        id: wifiButtonLoader
                        source: dbusNetwork.wirelessDevices.length > 0 ? "WifiCheckButton.qml": ""
                    }

                    Loader {
                        id: gsmButtonLoader
                        source: "GsmCheckButton.qml"
                    }

                    Loader {
                        id: vpnButtonLoader
                        source: "VpnCheckButton.qml"
                    }

                    Loader {
                        id: bluetoothLoader
                        source: "BluetoothCheckButton.qml"
                    }

                    DImageCheckButton{
                        anchors.verticalCenter: parent.verticalCenter
                        inactivatedNomralImage: "images/airplane_mode_off.png"
                        inactivatedHoverImage: inactivatedNomralImage
                        inactivatedPressImage: inactivatedNomralImage

                        activatedNomralImage: "images/airplane_mode_on.png"
                        activatedHoverImage: activatedNomralImage
                        activatedPressImage: activatedNomralImage
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
                                monitorObject.SetBrightness(monitorName, value)
                            }
                        }

                        Connections{
                            target: monitorObject
                            onBrightnessChanged: {
                                if(!brightnessSlider.pressed){
                                    brightnessSlider.value = brightnessDict[monitorName]
                                }
                            }
                        }

                        Timer{
                            running: true
                            interval: 200
                            onTriggered: {
                                brightnessSlider.value = brightnessDict[monitorName]
                            }
                        }

                    }
                }
            }

        }

    }
}
