import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property string devicePath: "/"
    property int deviceStatus: 20

    property string inConnectingApPath: "/"

    ListModel {
        id: accessPointsModel
    }

    onDeviceStatusChanged:{
        if(deviceStatus == 100){
            scanTimer.start()
        }
    }

    expanded: deviceStatus != 20
    header.sourceComponent: DBaseLine{

        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(nm.wirelessDevices.length < 2){
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
                nm.wirelessEnabled = checked
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
            delegate: WirelessItem {}
        }

        DBaseLine{
            visible: accessPointsModel.count > 0 ? false: true
            color: dconstants.contentBgColor
            leftLoader.sourceComponent: DssH3{
                text: "Scanning..."
            }
        }
    }

    Timer {
        id: scanTimer
        interval: 1000
        onTriggered: {
            var accessPoints = nm.GetAccessPoints(devicePath)
            wirelessDevicesExpand.inConnectingApPath = "/"
            accessPointsModel.clear()
            for(var i in accessPoints){
                var ap = accessPoints[i]
                accessPointsModel.append({
                    "devicePath": devicePath,
                    "apName": ap[0],
                    "apSecured": ap[1],
                    "apSignal": ap[2],
                    "apPath": ap[3],
                    "apConnected": ap[4]
                })
            }
        }
    }
}
