import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../widgets"

Column{
    id: wiredItem
    width: parent.width
    height: childrenRect.height

    property int deviceState: wiredDevices[index]["State"] // TODO
    property string devicePath: wiredDevices[index]["Path"]
    property string deviceVendor: wiredDevices[index]["Vendor"]
    property bool isUsbDevice: wiredDevices[index]["UsbDevice"]
    property string uuid: dbusNetwork.GetWiredConnectionUuid(devicePath)

    property var activeConnectionInfo: getActiveConnectionInfo(uuid)
    property bool isConnected: wiredDevices[index].State == nmDeviceStateActivated

    function activateThisConnection(){
        // TODO
        uuid = dbusNetwork.GetWiredConnectionUuid(devicePath)
        dbusNetwork.ActivateConnection(uuid, devicePath)
    }

    function gotoEditWiredConnection(){
        // TODO
        uuid = dbusNetwork.GetWiredConnectionUuid(devicePath)
        gotoEditConnection("editPage", uuid, wiredItem.devicePath)
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
                    gotoEditWiredConnection()
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
                normal_image: "../images/connected.png"
                hover_image: "../images/disconnect.png"
                visible: isConnected
                onClicked: {
                    dbusNetwork.DisconnectDevice(devicePath)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: wiredDevices[index].State >= nmDeviceStatePrepare && wiredDevices[index].State <= nmDeviceStateSecondaries
            }

            DLabel {
                width: 220      // TODO need a preferred value
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                elide: Text.ElideRight
                text: {
                    if (isUsbDevice && deviceVendor) {
                        return deviceVendor
                    }
                    if (index > 0) {
                        return dsTr("Wired Connection %1").arg(index + 1)
                    }
                    return dsTr("Wired Connection")
                }
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
                gotoEditWiredConnection()
            }
        }
    }

    DSeparatorHorizontal{}
}
