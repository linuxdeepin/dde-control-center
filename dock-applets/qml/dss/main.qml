import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import "../widgets/"

DockQuickWindow {
    id: root
    title: "DSS"
    appid: "AppletDss"
    icon: iconPath
    width: buttonRow.width + xEdgePadding * 2
    height: contentColumn.height + xEdgePadding * 2
    color: Qt.rgba(0, 0, 0, 0.85)

    property url iconPath: "images/icon.png"

    property var dbusDisplay: Display{}
    property var monitorObject: Monitor{ path: dbusDisplay.monitors[0] }
    property var brightnessDict: monitorObject.brightness
    property string monitorName: Object.keys(brightnessDict)[0]
    property int xEdgePadding: 10

    Component.onCompleted: root.show()

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
                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/wire_off.png"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/wire_on.png"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/wifi_off.png"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/wifi_on.png"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/3g_off.png"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/3g_on.png"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/vpn_off.png"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/vpn_on.png"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/bluetooth_off.png"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/bluetooth_on.png"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/airplane_mode_off.png"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/airplane_mode_on.png"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
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
