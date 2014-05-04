import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property string devicePath: "/"
    property string deviceHwAddr
    property int deviceStatus: 0

    property string inConnectingApPath: "/"

    Component.onCompleted: {
        for(var i in dbusNetwork.wirelessDevices){
            if(dbusNetwork.wirelessDevices[i][0] == wirelessDevicesExpand.devicePath){
                wirelessDevicesExpand.deviceStatus = dbusNetwork.wirelessDevices[i][1]
            }
        }
        if(!scanTimer.running){
            scanTimer.start()
        }
    }

    ListModel {
        id: accessPointsModel

        function getIndexByApPath(path){
            for(var i; i<count; i++){
                var obj = get(i)
                if(obj.apPath == path){
                    return i
                }
            }
            return -1
        }

        function getInsertPosition(apObj){
            var position = count
            for(var i; i<count; i++){
                var obj = get(i)
                if(apObj.Path != obj.apPath && apObj.Strength >= obj.apSignal){
                    position = i
                    break
                }
            }
            return position
        }
    }

    Connections{
        target: dbusNetwork
        onAccessPointAdded:{
            if(arg0 == devicePath){
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
                var apModelObj = accessPointsModel.get(index)
                apModelObj.apName = apObj.Ssid
                apModelObj.apSecured = apObj.Secured
                apModelObj.apSecuredInEap = apObj.SecuredInEap
                apModelObj.apSignal = apObj.Strength
                apModelObj.apPath = apObj.Path

                var insertPosition = accessPointsModel.getInsertPosition(apObj)
                if(insertPosition != index){
                    accessPointsModel.move(index, position, 1)
                }
            }
        }

        onDeviceStateChanged: {
            if(arg0 == devicePath){
                wirelessDevicesExpand.deviceStatus = arg1
                if(arg1 == 100){ // TODO
                    wirelessDevicesExpand.inConnectingApPath = "/"
                }
            }
        }
    }

    expanded: deviceStatus != 20 // TODO
    header.sourceComponent: DBaseLine{

        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(dbusNetwork.wirelessDevices.length < 2){
                    return dsTr("Wireless Device: %1").arg(deviceStatus)
                }
                else{
                    return dsTr("Wireless Device %1").arg(index + 1)
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

    content.sourceComponent: Item {
        width: parent.width
        height: childrenRect.height

        ListView {
            visible: accessPointsModel.count > 0 ? true: false
            width: parent.width
            height: childrenRect.height
            model: accessPointsModel
            delegate: WirelessItem {
                devicePath: wirelessDevicesExpand.devicePath
                deviceHwAddr: wirelessDevicesExpand.deviceHwAddr
            }
        }

        DBaseLine{
            visible: accessPointsModel.count > 0 ? false: true
            color: dconstants.contentBgColor
            leftMargin: 24
            leftLoader.sourceComponent: DssH2{
                font.pixelSize: 12
                text: dsTr("Scanning...")
            }
        }
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

    Timer {
        id: scanTimer
        interval: 100
        onTriggered: {
            var accessPoints = unmarshalJSON(dbusNetwork.GetAccessPoints(devicePath))
            wirelessDevicesExpand.inConnectingApPath = "/"
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
        }
    }
}
