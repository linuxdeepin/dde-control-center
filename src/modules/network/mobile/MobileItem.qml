import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../widgets"

Column{
    id: mobileItem
    width: parent.width
    height: childrenRect.height

    //device info
    property var mobileDevice
    property string devicePath: mobileDevice["Path"]
    property string deviceVendor: mobileDevice["Vendor"]
    property bool isConnected: mobileDevice.State == nmDeviceStateActivated
    property var deviceUniqueUuid: mobileDevice.UniqueUuid

    //connection info
    property var mobileConnectionName:{
        var connectionList = nmConnections[nmConnectionTypeMobile]
        for (var i = 0; i < connectionList.length; i ++){
            if (connectionList[i].Uuid == deviceUniqueUuid)
                return connectionList[i].Id
        }

        return dsTr("Restore the connection")
    }

    function activateThisConnection(){
        dbusNetwork.ActivateConnection(deviceUniqueUuid, devicePath)
    }

    function gotoEditMobileConnection(){
        gotoEditConnection("editPage", deviceUniqueUuid, devicePath)
    }

    DBaseLine {
        id: mobileLine

        property bool hovered: false
        property bool selected: false
        color: DConstants.contentBgColor

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true
            onEntered: parent.hovered = true
            onExited: parent.hovered = false

            onClicked: {
                // TODO
                if (isConnected){
                    gotoEditMobileConnection()
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
                on: mobileDevice.State >= nmDeviceStatePrepare && mobileDevice.State <= nmDeviceStateSecondaries
            }

            DLabel {
                width: 220      // TODO need a preferred value
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                elide: Text.ElideRight
                text: mobileConnectionName//dsTr("3G Connection")
                font.pixelSize: 12
                color: {
                    if(mobileLine.selected){
                        return DConstants.activeColor
                    }
                    else if(mobileLine.hovered){
                        return DConstants.hoverColor
                    }
                    else{
                        return DConstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: DArrowButton {
            onClicked: {
                gotoEditMobileConnection()
            }
        }
    }
}
