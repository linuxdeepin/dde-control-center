import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../../shared/"
import "../widgets"

Item {
    id: wirelessItem
    width: parent.width
    height: childrenRect.height

    property string devicePath: "/"
    property string deviceHwAddress: ""
    property int deviceState: nmDeviceStateUnknown
    property string activeAp: "/"
    property bool apConnected: activeAp == masterApPath && deviceState == nmDeviceStateActivated
    property string connectionPath
    property string uuid: ""
    property string masterApSsid: masterApInfo.Ssid
    property string masterApPath: masterApInfo.Path
    property bool masterApSecured: masterApInfo.Secured
    property bool masterApSecuredInEap: masterApInfo.SecuredInEap
    property int masterApStrength: masterApInfo.Strength

    function updateMasterApInfoWhenActivatingAp() {
        if (!apInfos) {
            // wireless item is removed
            return
        }
        if (apInfos.count == 0) {
            print("-> [warning] wireless item apInfos is empty", masterApInfo.Ssid, masterApInfo.Path)
            return
        }
        for(var i=0; i<apInfos.count; i++){
            // use the actived child ap as master ap
            if(apInfos.get(i).Path == activeAp){
                print("-> updateMasterApInfoWhenActivatingAp", activeAp, apInfos.get(i).Ssid, true)
                masterApInfo = apInfos.get(i)
                return
            }
        }
    }

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    Connections {
        target: dbusNetwork
        onConnectionsChanged: {
            updateWirelessConnectionInfo()
        }
    }

    Component.onCompleted: {
        updateWirelessConnectionInfo()
    }

    onActiveApChanged: {
        updateMasterApInfoWhenActivatingAp()
    }

    function updateWirelessConnectionInfo() {
        var conns = nmConnections[nmConnectionTypeWireless]
        var tmpPath = ""
        var tmpUuid = ""
        for (var i in conns) {
            if (masterApSsid == conns[i].Ssid) {
                if (conns[i].HwAddress == "" || conns[i].HwAddress == deviceHwAddress) {
                    tmpPath = conns[i].Path
                    tmpUuid = conns[i].Uuid
                    break
                }
            }
        }
        connectionPath = tmpPath
        uuid = tmpUuid
    }

    function activateWirelessConnection(){
        if (uuid == "" && masterApSecuredInEap) {
            // if access point under the security eap and there is
            // no valid connection for it, go to connection
            // property page and create connection session through
            // CreateConnectionForAccessPoint()
            gotoAddWirelessConnectionWizard()
        } else {
            // connection for current access point exists, just activate it
            wirelessItem.connectionPath = dbusNetwork.ActivateAccessPoint(uuid, masterApPath, devicePath)
        }
    }

    function editWirelessConnection(){
        if (uuid != "") {
            print("==> gotoEditWirelessConnection", uuid)
            gotoEditWirelessConnection()
        } else {
            print("==> gotoAddWirelessConnection")
            gotoAddWirelessConnection()
        }
    }

    function gotoEditWirelessConnection() {
        gotoEditConnection("editPage", uuid, devicePath)
    }
    function gotoAddWirelessConnection() {
        doGotoAddWirelessConnection("editPage")
    }
    function gotoAddWirelessConnectionWizard() {
        doGotoAddWirelessConnection("addWirelessPage")
    }
    function doGotoAddWirelessConnection(page) {
        stackView.push({
            "item": stackViewPages[page],
            "properties": { "connectionSession": createConnectionForAccessPoint(masterApPath, devicePath)},
            "destroyOnPop": true
        })
        stackView.currentItemId = page
    }

    DBaseLine {
        id: wirelessLine

        property bool hovered: false
        property bool selected: false

        MouseArea {
            z: -1
            anchors.fill:parent
            hoverEnabled: true
            visible: !networkModule.inPasswordInputting

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                if(!apConnected){
                    print("==> ActivateAccessPoint", uuid, masterApPath, devicePath)
                    activateWirelessConnection()
                    // TODO show waiting image imediately
                    // wirelessLine.leftLoader.item.waitingImageOn = true
                }
                else{
                    print("==> edit connection", uuid)
                    editWirelessConnection()
                }
            }
        }
        color: dconstants.contentBgColor

        leftLoader.sourceComponent: Item {
            height: parent.height
            width: childrenRect.width
            property alias waitingImageOn: waitingImage.on
            DImageButton {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "../images/connected.png"
                hover_image: "../images/disconnect.png"
                visible: apConnected
                onClicked: {
                    dbusNetwork.DisconnectDevice(devicePath)
                }
            }

            WaitingImage {
                id: waitingImage
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: activeAp == masterApPath && deviceState >= nmDeviceStatePrepare && deviceState <= nmDeviceStateSecondaries
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                text: masterApSsid
                font.pixelSize: 12
                color: {
                    if(wirelessLine.selected){
                        return dconstants.activeColor
                    }
                    else if(wirelessLine.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: Row {
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            Image {
                source: {
                    var power = masterApStrength
                    var secure = masterApSecured ? "-secure": ""
                    var imageNumber = 100

                    if (power <= 5){
                        imageNumber = 0
                    }
                    else if (power <= 25) {
                        imageNumber = 25
                    }
                    else if (power <= 50) {
                        imageNumber = 50
                    }
                    else if (power <= 75) {
                        imageNumber = 75
                    }
                    else if (power <= 100) {
                        imageNumber = 100
                    }

                    return "../images/wifi-signal-" + imageNumber + secure + ".svg"
                }
            }

            DArrowButton {
                onClicked: {
                    editWirelessConnection()
                }
            }
        }
    }

    PasswordBox {
        id: passwordArea
        anchors.top: wirelessLine.bottom
        anchors.topMargin: 0 - arrowHeight
        path: wirelessItem.connectionPath
    }
}
