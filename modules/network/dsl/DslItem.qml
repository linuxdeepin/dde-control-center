import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../widgets"

Item {
    width: parent.width
    height: childrenRect.height
    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    property int connectionStatus: 100

    property var infos: dslConnections[index]

    /// TODO: activeConnectionInfo is always null
    property var activeConnectionInfo: getActiveConnectionInfo(infos.Uuid)
    // property bool isConnected: activeConnectionInfo && getBool(activeConnectionInfo.dsl) && activeConnectionInfo.State == nmActiveConnectionStateActivated
    property bool isConnected: activeConnectionInfo && activeConnectionInfo.State == nmActiveConnectionStateActivated

    // Component.onCompleted: {
        // print(activeConnectionInfo)
    // }
    
    function getBool(s){
        if(typeof(s) == "undefined"){
            return false
        }
        else{
            return s
        }
    }

    function gotoEditDslConnection(){
        gotoEditConnection("editPage", infos.Uuid, getDevicePath())
    }

    function getDevicePath(){
        var wiredDevices = nmDevices[nmConnectionTypeWired]
        var wirelessDevices = nmDevices[nmConnectionTypeWireless]
        if(wiredDevices){
            for(var i in wiredDevices){
                if(wiredDevices[i].State == nmDeviceStateActivated){
                    return wiredDevices[i].Path
                }
            }
            return wiredDevices[0].Path
        }
        else if(wirelessDevices){
            for(var i in wirelessDevices){
                if(wirelessDevices[i].State == nmDeviceStateActivated){
                    return wirelessDevices[i].Path
                }
            }
            return wirelessDevices[0].Path
        }

        return "/"
    }

    function activateThisConnection(){
        var devicePath = getDevicePath()
        print("DSL Device Path:", devicePath) 
        dbusNetwork.ActivateConnection(infos.Uuid, devicePath)
    }

    DBaseLine {
        id: lineBox
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
                if (isConnected){
                    gotoEditDslConnection()
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
                    // dbusNetwork.DeactivateConnection(infos.Uuid) // TODO
                    dbusNetwork.DisconnectDevice(getDevicePath())
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                // on: activeConnectionInfo && activeConnectionInfo.Uuid == infos.Uuid && getBool(activeConnectionInfo.dsl) && activeConnectionInfo.State == nmActiveConnectionStateActivating
                on: activeConnectionInfo && activeConnectionInfo.Uuid == infos.Uuid && activeConnectionInfo.State == nmActiveConnectionStateActivating
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: infos.Id
                font.pixelSize: 12
                color: {
                    if(lineBox.selected){
                        return dconstants.activeColor
                    }
                    else if(lineBox.hovered){
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
                gotoEditDslConnection()
            }
        }
    }

    PasswordBox {
        id: passwordArea
        anchors.top: lineBox.bottom
        anchors.topMargin: 0 - arrowHeight
        uuid: infos.Uuid
        path: infos.Path
    }
}
