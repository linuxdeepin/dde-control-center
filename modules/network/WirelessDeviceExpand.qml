import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property string devicePath: "/"
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

        function getInsertPosition(apProperty){
            var position = count
            for(var i; i<count; i++){
                var obj = get(i)
                if(apProperty[3] != obj.apPath && apProperty[2] >= obj.apSignal){
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
                var apProperty = dbusNetwork.GetAccessPointProperty(arg1)
                var insertPosition = accessPointsModel.getInsertPosition(apProperty)
                accessPointsModel.insert(insertPosition, {
                    "apName": apProperty[0],
                    "apSecured": apProperty[1],
                    "apSignal": apProperty[2],
                    "apPath": apProperty[3]
                })
            }
        }

        onAccessPointRemoved:{
            if(arg0 == devicePath){
                var index = accessPointsModel.getIndexByApPath(arg1)
                if(index != -1){
                    accessPointsModel.remove(index, 1)
                }
            }
        }

        onAccessPointPropertiesChanged: {
            if(arg0 == devicePath){
                var newApProperty = dbusNetwork.GetAccessPointProperty(arg1)
                var index = accessPointsModel.getIndexByApPath(arg1)
                var apPropertyObject = accessPointsModel.get(index)
                for(var i in newApProperty){
                    apPropertyObject.apName = newApProperty[0]
                    apPropertyObject.apSecured = newApProperty[1]
                    apPropertyObject.apSignal = newApProperty[2]
                    apPropertyObject.apPath = newApProperty[3]
                }

                var insertPosition = accessPointsModel.getInsertPosition(newApProperty)
                if(insertPosition != index){
                    accessPointsModel.move(index, position, 1)
                }
            }
        }

        onDeviceStateChanged: {
            print("onDeviceStateChanged:", arg0, arg1)
            wirelessDevicesExpand.deviceStatus = arg1
            if(arg1 == 100){
                wirelessDevicesExpand.inConnectingApPath = "/"
            }
            if(arg0 == devicePath){
                wirelessDevicesExpand.deviceStatus = arg1
            }
        }
    }

    expanded: deviceStatus != 20
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
            }
        }
    }

    function sortModel()
    {
        var n;
        var i;
        for (n=0; n < accessPointsModel.count; n++)
            for (i=n+1; i < accessPointsModel.count; i++)
            {
                if (accessPointsModel.get(n).apSignal < accessPointsModel.get(i).apSignal)
                {
                    accessPointsModel.move(i, n, 1);
                    n=0; // Repeat at start since I can't swap items i and n
                }
            }
    }

    Timer {
        id: scanTimer
        interval: 100
        onTriggered: {
            var accessPoints = dbusNetwork.GetAccessPoints(devicePath)
            wirelessDevicesExpand.inConnectingApPath = "/"
            accessPointsModel.clear()

            for(var i in accessPoints){
                var ap = dbusNetwork.GetAccessPointProperty(accessPoints[i])
                accessPointsModel.append({
                    "apName": ap[0],
                    "apSecured": ap[1],
                    "apSignal": ap[2],
                    "apPath": ap[3]
                })
            }
            wirelessDevicesExpand.sortModel()
        }
    }
}
