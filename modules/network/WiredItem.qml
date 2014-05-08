import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column{
    id: wiredDevicesItem
    width: parent.width
    height: childrenRect.height

    property int wiredDeviceSignal: wiredDevices[index]["State"]
    property string wiredDevicePath: wiredDevices[index]["Path"]
    property string uuid: dbusNetwork.GetWiredConnectionUuid(wiredDevicePath)

    function activateThisConnection(){
        dbusNetwork.ActivateConnection(uuid, wiredDevicePath)
    }

    function goToEditConnection(){
        // print("wired device path", dbusNetwork.wiredDevices[index][0]) // TODO test
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "uuid": uuid, "devicePath": wiredDevicePath },
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }

    DBaseLine {
        id: wiredLine

        property bool hovered: false
        property bool selected: false
        color: dconstants.contentBgColor

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                // TODO
                if (wiredDeviceSignal == 100){
                    goToEditConnection()
                }
                else{
                    activateThisConnection()
                }
            }
        }

        leftLoader.sourceComponent: Item {
            width: parent.width
            height: parent.height

            DImageButton {
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "img/check_1.png"
                hover_image: "img/check_2.png"
                visible: wiredDeviceSignal == 100
                onClicked: {
                    dbusNetwork.DisconnectDevice(wiredDevicePath)
                }
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Wired Conection %1").arg(index+1)
                font.pixelSize: 12
                color: {
                    if(wiredLine.selected){
                        return dconstants.activeColor
                    }
                    else if(wiredLine.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: DArrowButton {
            onClicked: {
                goToEditConnection()
            }
        }
    }
    
    DSeparatorHorizontal{}
}
