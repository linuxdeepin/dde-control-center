import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

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

            Row {
                width: parent.width
                height: brightnessSlider.height
                spacing: 8

                Image{
                    source: "images/light.png"
                }

                DSliderEnhanced{
                    id: brightnessSlider
                    width: parent.width - 30
                    height: 26

                    min: 0
                    max: 1.0
                    init: brightnessDict[Object.keys(brightnessDict)[0]]

                    onValueConfirmed: {
                        for(var key in brightnessDict){
                            monitorObject.SetBrightness(key, value)
                        }
                    }

                    valueDisplayVisible: false
                }
            }
        }

    }

}
