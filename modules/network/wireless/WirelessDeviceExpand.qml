import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property string devicePath: "/"
    property string deviceHwAddress
    property string activeAp: "/"
    property int deviceState: nmDeviceStateUnknown
    property bool deviceEnabled: false
    expanded: deviceEnabled

    Component.onCompleted: {
        deviceEnabled = deviceState > nmDeviceStateDisconnected
        if(!scanTimer.running){
            scanTimer.start()
        }
    }

    ListModel {
        id: accessPointsModel

        function getIndexByApPath(path){
            for(var i=0; i<count; i++){
                var obj = get(i)
                if(obj.apPath == path){
                    return i
                }
            }
            return -1
        }

        function getInsertPosition(apObj){
            for(var i=0; i<count; i++){
                var obj = get(i)
                if(apObj.Path != obj.apPath && apObj.Strength >= obj.apSignal){
                    return i
                }
            }
            return count
        }
    }

    Connections{
        target: dbusNetwork
        onAccessPointAdded:{
            if(arg0 == devicePath){
                // print("onAccessPointAdded:", arg0, arg1) // TODO test
                var apObj = unmarshalJSON(arg1)
                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                if(index == -1){
                    var insertPosition = accessPointsModel.getInsertPosition(apObj)
                    accessPointsModel.insert(insertPosition, {
                        "apName": apObj.Ssid,
                        "apSecured": apObj.Secured,
                        "apSecuredInEap": apObj.SecuredInEap,
                        "apSignal": apObj.Strength,
                        "apPath": apObj.Path
                    })
                }
            }
        }

        onAccessPointRemoved:{
            if(arg0 == devicePath){
                // print("onAccessPointRemoved:", arg0, arg1) // TODO test
                var apObj = unmarshalJSON(arg1)
                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                if(index != -1){
                    accessPointsModel.remove(index, 1)
                }
            }
        }

        onAccessPointPropertiesChanged: {
            if(arg0 == devicePath){
                var apObj = unmarshalJSON(arg1)
                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                if (index != -1){
                    var apModelObj = accessPointsModel.get(index)
                    apModelObj.apName = apObj.Ssid
                    apModelObj.apSecured = apObj.Secured
                    apModelObj.apSecuredInEap = apObj.SecuredInEap
                    apModelObj.apSignal = apObj.Strength
                    apModelObj.apPath = apObj.Path
                }
            }
        }
    }

    header.sourceComponent: DBaseLine{

        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(wirelessDevices.length < 2){
                    return dsTr("Wireless Network")
                }
                else{
                    return dsTr("Wireless Network %1").arg(index + 1)
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: scanTimer.restart()
            }
        }

        rightLoader.sourceComponent: DSwitchButton{
            property var lastActiveUuid
            checked: wirelessDevicesExpand.expanded
            onClicked: {
                if (checked) {
                    // enable device
                    deviceEnabled = true
                    dbusNetwork.wirelessEnabled = true
                    if (lastActiveUuid) {
                        dbusNetwork.ActivateConnection(lastActiveUuid, devicePath)
                    }
                } else {
                    // disable device
                    deviceEnabled = false
                    lastActiveUuid = getDeviceActiveConnection(devicePath)
                    if (deviceState > nmDeviceStateDisconnected && deviceState <= nmDeviceStateActivated) {
                        dbusNetwork.DisconnectDevice(devicePath)
                    }
                }
            }
        }
    }

    content.sourceComponent: Column {
        width: parent.width

        ListView {
            width: parent.width
            height: childrenRect.height
            boundsBehavior: Flickable.StopAtBounds
            model: accessPointsModel
            delegate: WirelessItem {
                devicePath: wirelessDevicesExpand.devicePath
                deviceHwAddress: wirelessDevicesExpand.deviceHwAddress
                activeAp: wirelessDevicesExpand.activeAp
                deviceState: wirelessDevicesExpand.deviceState
            }
        }

        DBaseLine {
            id: hiddenNetworkLine
            color: dconstants.contentBgColor

            property bool hovered: false

            leftLoader.sourceComponent: DssH2 {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Connect Hidden Access Point")
                font.pixelSize: 12
                color: hiddenNetworkLine.hovered ? dconstants.hoverColor : dconstants.fgColor
            }
            rightLoader.sourceComponent: DArrowButton {
                onClicked: gotoConnectHiddenAP()
            }

            MouseArea {
                z: -1
                anchors.fill:parent
                hoverEnabled: true
                onEntered: parent.hovered = true
                onExited: parent.hovered = false
                onClicked: gotoConnectHiddenAP()
            }
        }

        DSeparatorHorizontal {
            visible: false
        }

        DBaseLine {
            id: wifiHotspotLine
            color: dconstants.contentBgColor
            visible: true

            property var hotspotInfo: {
                var info = null
                var infos = nmConnections[nmConnectionTypeWirelessHotspot]
                for (var i in infos) {
                    if (infos[i].HwAddress == "" || infos[i].HwAddress == deviceHwAddress) {
                        info = infos[i]
                        break
                    }
                }
                return info
            }

            property bool hovered: false

            leftLoader.sourceComponent: DssH2 {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: wifiHotspotLine.hotspotInfo ? dsTr("Hotspot ") + wifiHotspotLine.hotspotInfo.Id : dsTr("Create Access Point")
                font.pixelSize: 12
                color: wifiHotspotLine.hovered ? dconstants.hoverColor : dconstants.fgColor
            }
            rightLoader.sourceComponent: DArrowButton {
                onClicked: gotoCreateAP(wifiHotspotLine.hotspotInfo)
            }

            MouseArea {
                z: -1
                anchors.fill:parent
                hoverEnabled: true
                onEntered: parent.hovered = true
                onExited: parent.hovered = false
                onClicked: {
                    if(wifiHotspotLine.hotspotInfo){
                        dbusNetwork.ActivateConnection(wifiHotspotLine.hotspotInfo.Uuid, devicePath)
                    }
                    else{
                        gotoCreateAP(wifiHotspotLine.hotspotInfo)
                    }
                }
            }
        }
    }

    // TODO timer should be removed
    Timer {
        id: sortModelTimer
        interval: 1000
        repeat: true
        onTriggered: {
            wirelessDevicesExpand.sortModel()
        }
    }

    // TODO timer should be removed
    Timer {
        id: scanTimer
        interval: 100
        onTriggered: {
            var accessPoints = unmarshalJSON(dbusNetwork.GetAccessPoints(devicePath))
            accessPointsModel.clear()

            for(var i in accessPoints){
                // TODO ap
                var apObj = accessPoints[i]
                accessPointsModel.append({
                    "apName": apObj.Ssid,
                    "apSecured": apObj.Secured,
                    "apSecuredInEap": apObj.SecuredInEap,
                    "apSignal": apObj.Strength,
                    "apPath": apObj.Path
                })
            }
            wirelessDevicesExpand.sortModel()
            sortModelTimer.start()
        }
    }

    function gotoConnectHiddenAP(){
        var page = "addWirelessPage"
        stackView.push({
            "item": stackViewPages[page],
            "properties": {"connectionSession": createConnection(nmConnectionTypeWireless, devicePath), "title": dsTr("Connect to hidden access point")},
            "destroyOnPop": true
        })
        stackView.currentItemId = page
    }

    function gotoCreateAP(hotspotInfo){
        var page = "wifiHotspotPage"
        stackView.push({
            "item": stackViewPages[page],
            "properties": { "hotspotInfo": hotspotInfo, "devicePath": devicePath},
            "destroyOnPop": true
        })
        stackView.currentItemId = page
    }

    function sortModel()
    {
        var n;
        var i;
        for (n=0; n < accessPointsModel.count; n++){
            for (i=n+1; i < accessPointsModel.count; i++)
            {
                if (accessPointsModel.get(n).apSignal < accessPointsModel.get(i).apSignal)
                {
                    accessPointsModel.move(i, n, 1);
                    n=0; // Repeat at start since I can't swap items i and n
                }
            }
        }
    }
}
