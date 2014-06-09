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
        deviceEnabled = deviceState >= nmDeviceStateDisconnected
        if(!scanTimer.running){
            scanTimer.start()
        }
    }
    onDeviceStateChanged: {
        deviceEnabled = deviceState >= nmDeviceStateDisconnected
    }

    ListModel {
        id: accessPointsModel
        
        function getIndexBySsid(ssid){
            for(var i=0; i<count; i++){
                var item = get(i)
                if(item.masterApInfo.Ssid == ssid){
                    return i
                }
            }
            return -1
        }
        
        function getIndexByPath(path){
            for(var i=0; i<count; i++){
                var item = get(i)
                for(var j=0; j<item.apInfos.count; j++){
                    if(item.apInfos.get(j).Path == path){
                        return i
                    }
                }
            }
            return -1
        }

        function getInsertPosition(apInfo){
            // check if access point with same ssid already exists
            var index = getIndexBySsid(apInfo.Ssid)
            for(var i=0; i<count; i++){
                var item = get(i)
                if(apInfo.Strength >= item.masterApInfo.Strength){
                    return i
                }
            }
            return count
        }

        function addOrUpdateApItem(apInfo){
            // print("-> addOrUpdateApItem", apInfo.Ssid, apInfo.Path) // TODO test
            var index = getIndexBySsid(apInfo.Ssid)
            if(index == -1){
                addApItem(apInfo)
            }
            else{
                updateApItem(index, apInfo)
            }
        }

        function addApItem(apInfo){
            var insertPosition = getInsertPosition(apInfo)
            // print("-> addApItem", insertPosition, apInfo.Ssid, apInfo.Path) // TODO test
            insert(insertPosition, {"masterApInfo": apInfo, "apInfos": [apInfo]})
        }

        function updateApItem(index, apInfo){
            // print("-> updateApItem", index, apInfo.Ssid, apInfo.Path) // TODO test
            
            // check if ap already exists as a child, if is just
            // update it, or append it to apInfos
            var item = get(index)
            var childApIndex = -1
            for(var i=0; i<item.apInfos.count; i++){
                if(item.apInfos.get(i).Path == apInfo.Path){
                    childApIndex = i
                    break
                }
            }
            if (childApIndex != -1) {
                item.apInfos.set(i, apInfo)
            } else {
                item.apInfos.append(apInfo)
            }
            updateItemMasterApInfo(index)
        }

        function removeApItem(index, apInfo) {
            // print("-> removeApItem", index, apInfo.Ssid, apInfo.Path) // TODO test
            var item = get(index)
            for(var i=0; i<item.apInfos.count; i++){
                if(item.apInfos.get(i).Path == apInfo.Path){
                    item.apInfos.remove(i, 1)
                    break
                }
            }
            if(item.apInfos.count == 0){
                remove(index, 1)
            } else {
                updateItemMasterApInfo(index)
            }
        }

        function updateItemMasterApInfo(index) {
            var item = get(index)
            if (item.apInfos.count == 0) {
                print("-> [warning] ap item apInfos is empty when update index", index)
                return
            }
            var apInfo = item.apInfos.get(0)
            for(var i=0; i<item.apInfos.count; i++){
                // use the actived child ap as master ap
                if(item.apInfos.get(i).Path == activeAp){
                    apInfo = item.apInfos.get(i)
                    break
                }
                // or use the strengthest signal ap
                if(item.apInfos.get(i).Strength > apInfo.Strength){
                    apInfo = item.apInfos.get(i)
                }
            }
            item.masterApInfo = apInfo
        }
    }

    Connections{
        target: dbusNetwork
        onAccessPointAdded:{
            if(arg0 == devicePath){
                // print("onAccessPointAdded:", arg0, arg1) // TODO test
                var apInfo = unmarshalJSON(arg1)
                accessPointsModel.addOrUpdateApItem(apInfo)
            }
        }

        onAccessPointRemoved:{
            if(arg0 == devicePath){
                // print("onAccessPointRemoved:", arg0, arg1) // TODO test
                var apInfo = unmarshalJSON(arg1)
                var index = accessPointsModel.getIndexByPath(apInfo.Path)
                if(index != -1){
                    accessPointsModel.removeApItem(index, apInfo)
                }
            }
        }

        onAccessPointPropertiesChanged: {
            if(arg0 == devicePath){
                var apInfo = unmarshalJSON(arg1)
                accessPointsModel.addOrUpdateApItem(apInfo)
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
                    if (!dbusNetwork.wirelessEnabled) {
                        dbusNetwork.wirelessEnabled = true
                    } else {
                        if (lastActiveUuid) {
                            dbusNetwork.ActivateConnection(lastActiveUuid, devicePath)
                        }
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

    // TODO
    Timer {
        id: sortModelTimer
        interval: 1000
        repeat: true
        onTriggered: {
            wirelessDevicesExpand.sortModel()
        }
    }

    // TODO
    Timer {
        id: scanTimer
        interval: 100
        onTriggered: {
            var accessPoints = unmarshalJSON(dbusNetwork.GetAccessPoints(devicePath))
            accessPointsModel.clear()
            for(var i in accessPoints){
                accessPointsModel.addOrUpdateApItem(accessPoints[i])
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
                if (accessPointsModel.get(n).masterApInfo.Strength + 5 < accessPointsModel.get(i).masterApInfo.Strength)
                {
                    accessPointsModel.move(i, n, 1);
                    n=0; // Repeat at start since I can't swap items i and n
                }
            }
        }
    }
}
