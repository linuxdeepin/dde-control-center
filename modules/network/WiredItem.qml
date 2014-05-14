import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "widgets"

Column{
    id: wiredDevicesItem
    width: parent.width
    height: childrenRect.height

    property int wiredDeviceSignal: wiredDevices[index]["State"]
    property string wiredDevicePath: wiredDevices[index]["Path"]
    property string uuid: dbusNetwork.GetWiredConnectionUuid(wiredDevicePath)

    property var activeConnectionInfo: getActiveConnectionInfo(uuid)
    // property bool isConnected: activeConnectionInfo && !activeConnectionInfo.Vpn && activeConnectionInfo.State == nmActiveConnectionStateActivated
    property bool isConnected: wiredDevices[index].State == nmDeviceStateActivated

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
            onEntered: parent.hovered = true
            onExited: parent.hovered = false

            onClicked: {
                // TODO
                if (isConnected){
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
                normal_image: "images/connected.png"
                hover_image: "images/disconnect.png"
                visible: isConnected
                onClicked: {
                    dbusNetwork.DisconnectDevice(wiredDevicePath)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: wiredDevices[index].State >= nmDeviceStatePrepare && wiredDevices[index].State <= nmDeviceStateSecondaries
                // on: activeConnectionInfo && activeConnectionInfo.Uuid == uuid && !activeConnectionInfo.Vpn && activeConnectionInfo.State == nmActiveConnectionStateActivating
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Wired Connection %1").arg(index+1)
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
