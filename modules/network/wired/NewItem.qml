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
    property bool isConnected: {
        if(itemType == "wired"){
            return deviceState == nmDeviceStateActivated
        }
        else{
            return activeDslConnetionInfo && activeDslConnetionInfo.State == nmActiveConnectionStateActivated
        }
    }
    property bool isWaiting: {
        if(itemType == "wired"){
            return deviceState >= nmDeviceStatePrepare && deviceState <= nmDeviceStateSecondaries
        }
        else{
            return activeDslConnetionInfo && activeDslConnetionInfo.State == nmActiveConnectionStateActivating
        }
    }
    property string lableName: {
        if(itemType == "wired"){
            return dsTr("Wired Connection %1").arg(connIndex+1)
        }
        else{
            return dslInfo.Id
        }
    }

    // for wired
    property var currentDeviceInfo: wiredDevices[connIndex]
    property int deviceState: currentDeviceInfo.State
    property string devicePath: currentDeviceInfo.Path
    property string uuid: dbusNetwork.GetWiredConnectionUuid(devicePath)

    // for DSL
    property var dslInfo: dslIndex == -1 ? null : dslConnections[dslIndex]
    property var activeDslConnetionInfo: {
        if(dslInfo){
            return getActiveConnectionInfo(dslInfo.Uuid)
        }
        return null
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
            dbusNetwork.ActivateConnection(dslInfo.Uuid, devicePath)
        }
    }

    function gotoEditWiredConnection(){
        if(itemType == "wired"){
            gotoEditConnection("editPage", uuid, wiredItem.devicePath)
        }
        else if(itemType == "dsl"){
            gotoEditConnection("editPage", dslInfo.Uuid, "/")
        }
    }

    function disconnectAction(){
        if(itemType == "wired"){
            print_info("disconnect wired connection: " + devicePath)
            dbusNetwork.DisconnectDevice(devicePath)
        }
        else if(itemType == "dsl"){
            print_info("disconnect dsl connection: " + devicePath)
            dbusNetwork.DeactivateConnection(dslInfo.Uuid)
        }
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
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: lableName
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
    
    //DSeparatorHorizontal{}
}
