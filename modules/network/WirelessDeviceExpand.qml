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
    }

    Connections{
        target: dbusNetwork
        onAccessPointAdded:{
            if(arg0 == devicePath){
                print("AccessPointAdded:", arg0, arg1)
                accessPointsModel.append({
                    "apName": arg1[0],
                    "apSecured": arg1[1],
                    "apSignal": arg1[2],
                    "apPath": arg1[3]
                })
            }
        }
        onAccessPointRemoved:{
            if(arg0 == devicePath){
                print("AccessPointRemoved:", arg0, arg1)
                var i = 0;
                while(i < accessPointsModel.count){
                    var accessPointInfo = accessPointsModel.get(i);
                    if(arg1 == accessPointInfo.apPath){
                        accessPointsModel.remove(i, 1);
                        break;
                    }
                    i++;
                }
            }
        }
        onDeviceStateChanged: {
            wirelessDevicesExpand.deviceStatus = arg1
            if(arg1 == 100){
                wirelessDevicesExpand.inConnectingApPath = "/"
            }
            if(arg0 == devicePath){
                wirelessDevicesExpand.deviceStatus = arg1
            }
        }
        onAccessPointPropertiesChanged: {
            print("onAccessPointPropertiesChanged:", arg0, arg1)
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

    Timer {
        id: scanTimer
        interval: 100
        onTriggered: {
            var accessPoints = dbusNetwork.GetAccessPoints(devicePath)
            wirelessDevicesExpand.inConnectingApPath = "/"
            accessPointsModel.clear()
            for(var i in accessPoints){
                var ap = accessPoints[i]
                accessPointsModel.append({
                    "apName": ap[0],
                    "apSecured": ap[1],
                    "apSignal": ap[2],
                    "apPath": ap[3],
                })
            }
        }
    }
}
