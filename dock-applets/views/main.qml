import QtQuick 2.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Rectangle {
    id: root
    width: 320; height: 100
    color: Qt.rgba(0, 0, 0, 0.85)

    property url iconPath: "images/icon.png"

    property var dbusDisplay: Display{}
    property var monitorObject: Monitor{ path: dbusDisplay.monitors[0] }
    property var brightnessDict: monitorObject.brightness

    Item {
        anchors.centerIn: parent
        width: parent.width - 20
        height: parent.height - 20

        Column {
            width: parent.width
            spacing: 16
            Row {
                //anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16
                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/wifi_off.svg"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/wifi_on.svg"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/wired_network_off.svg"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/wired_network_on.svg"
                    activatedHoverImage: activatedNomralImage
                    activatedPressImage: activatedNomralImage
                    active: true
                }

                DImageCheckButton{
                    anchors.verticalCenter: parent.verticalCenter
                    inactivatedNomralImage: "images/gsm_3g_off.svg"
                    inactivatedHoverImage: inactivatedNomralImage
                    inactivatedPressImage: inactivatedNomralImage

                    activatedNomralImage: "images/gsm_3g_on.svg"
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
            }

            Row {
                width: parent.width
                height: brightnessSlider.height
                spacing: 8

                Image{
                    width: 26
                    height: 26
                    source: "images/brightness.png"
                }

                DSliderEnhanced{
                    id: brightnessSlider
                    width: 250
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
