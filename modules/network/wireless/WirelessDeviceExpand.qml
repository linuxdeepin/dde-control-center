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
    property int deviceStatus: 0

    Component.onCompleted: {
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

    expanded: deviceStatus != 20 // TODO
    header.sourceComponent: DBaseLine{

        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(wirelessDevices.length < 2){
                    return dsTr("Wireless Network")
                }
                else{
                    return dsTr("Wireless Network %1").arg(index + 1).arg(deviceStatus)
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: scanTimer.restart()
            }
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: wirelessDevicesExpand.expanded
            onClicked: {
                dbusNetwork.wirelessEnabled = checked
                accessPointsModel.clear()
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
                deviceStatus: wirelessDevicesExpand.deviceStatus
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
            visible: false

            property var hotspotInfo: {
                var infos = nmConnections[nmConnectionTypeWirelessAdhoc]
                if(infos){
                    return infos[0]
                }
                else{
                    return null
                }
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

    Timer {
        id: sortModelTimer
        interval: 1000
        repeat: true
        onTriggered: {
            wirelessDevicesExpand.sortModel()
        }
    }

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
        stackView.push({
            "item": stackViewPages["hiddenAp"],
            "properties": { "devicePath": devicePath},
            "destroyOnPop": true
        })
        stackView.currentItemId = "hiddenAp"
    }

    function gotoCreateAP(hotspotInfo){
        stackView.push({
            "item": stackViewPages["wifiHotspot"],
            "properties": { "hotspotInfo": hotspotInfo, "devicePath": devicePath},
            "destroyOnPop": true
        })
        stackView.currentItemId = "wifiHotspot"
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
