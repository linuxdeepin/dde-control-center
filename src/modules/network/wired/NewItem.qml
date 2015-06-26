import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../widgets"

Column{
    id: wiredItem
    width: parent.width
    height: childrenRect.height
    property string itemType: dslIndex == -1 ? "wired" : "dsl"

    /** variable no changed after initialized **/
    property int connIndex: conn_index
    property var dslIndex: dsl_index

    /** dynamic variable after initialized **/
    // TODO
    property bool isConnected: {
        if(itemType == "wired"){
            var activeWiredConnectionInfo = getActiveConnectionInfo(wiredItem.uuid)
            return activeWiredConnectionInfo && activeWiredConnectionInfo.State == nmActiveConnectionStateActivated
            // return deviceState == nmDeviceStateActivated
        }
        else{
            return isActivedConnection(devicePath, dslInfoUuid)
        }
    }
    property bool isWaiting: {
        if(itemType == "wired"){
            var activeWiredConnectionInfo = getActiveConnectionInfo(wiredItem.uuid)
            // return deviceState >= nmDeviceStatePrepare && deviceState <= nmDeviceStateSecondaries
            return activeWiredConnectionInfo && activeWiredConnectionInfo.State == nmActiveConnectionStateActivating
        }
        else{
            return isActivatingConnection(devicePath, dslInfoUuid)
        }
    }
    property string lableName: {
        if(itemType == "wired"){
//            if (isUsbDevice && deviceVendor) {
//                return deviceVendor
//            }
            if (index > 0) {
                return dsTr("Wired Connection %1").arg(index + 1)
            }
            return dsTr("Wired Connection")
        }
        else{
            return dslInfoId
        }
    }

    // for wired
    property var currentDeviceInfo: wiredDevices[connIndex]
    property int deviceState: currentDeviceInfo.State
    property string devicePath: currentDeviceInfo.Path
    property string deviceVendor: currentDeviceInfo.Vendor
    property bool isUsbDevice: currentDeviceInfo.UsbDevice
    property string uuid: dbusNetwork.GetWiredConnectionUuid(devicePath)

    // for DSL
    property var dslInfo: dslIndex == -1 ? null : dslConnections[dslIndex]
    property var dslInfoId:{
        if (dslInfo == null)
            return ""
        else
            return dslInfo.Id
    }
    property var dslInfoUuid: {
        if (dslInfo == null)
            return ""
        else
            return dslInfo.Uuid
    }

    // functions
    function print_info(s){
        print("==> [info]", s)
    }

    function activateThisConnection(){
        if(itemType == "wired"){
            print_info("activate wired connection: " + devicePath)
            dbusNetwork.ActivateConnection(uuid, devicePath)
        }
        else if(itemType == "dsl"){
            print_info("activate dsl connection: " + devicePath)
            dbusNetwork.ActivateConnection(dslInfoUuid, devicePath)
        }
    }

    function gotoEditWiredConnection(){
        if(itemType == "wired"){
            gotoEditConnection("editPage", uuid, wiredItem.devicePath)
        }
        else if(itemType == "dsl"){
            // gotoEditConnection("editPage", dslInfoUuid, "/") // TODO
            gotoEditConnection("editPage", dslInfoUuid, wiredItem.devicePath)
        }
    }

    function disconnectAction(){
        print_info("disconnect wired connection: " + devicePath)
        dbusNetwork.DisconnectDevice(devicePath)
        // TODO
        // if(itemType == "wired"){
        //     print_info("disconnect wired connection: " + devicePath)
        //     dbusNetwork.DisconnectDevice(devicePath)
        // }
        // else if(itemType == "dsl"){
        //     print_info("disconnect dsl connection: " + devicePath)
        //     dbusNetwork.DeactivateConnection(dslInfoUuid)
        // }
    }

    DBaseLine {
        id: wiredLine

        property bool hovered: false
        property bool selected: false
        color: DPalette.contentBgColor

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true
            onEntered: parent.hovered = true
            onExited: parent.hovered = false

            onClicked: {
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
                    disconnectAction()
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: isWaiting
            }

            DLabel {
                width: 220      // TODO need a preferred value
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: lableName
                font.pixelSize: 12
                elide: Text.ElideRight
                color: {
                    if(wiredLine.selected){
                        return DPalette.activeColor
                    }
                    else if(wiredLine.hovered){
                        return DPalette.hoverColor
                    }
                    else{
                        return DPalette.fgColor
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

    //DSeparatorHorizontal{}
}
