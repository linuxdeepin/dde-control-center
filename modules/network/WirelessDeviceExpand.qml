import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property variant dev
    property variant accessPoints

    expanded: dev[1] != 20
    header.sourceComponent: DBaseLine{

        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(nm.wirelessDevices.length < 2){
                    return dsTr("Wireless Device")
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
            visible: wirelessDevicesExpand.accessPoints ? true: false
            width: parent.width
            height: childrenRect.height
            model: wirelessDevicesExpand.accessPoints
            delegate: WirelessItem {
                devicePath: dev[0]
                accessPoint: wirelessDevicesExpand.accessPoints[index]
            }
            Component.onCompleted: {
            }
        }
        DBaseLine{
            visible: wirelessDevicesExpand.accessPoints ? false: true
            color: dconstants.contentBgColor
            leftLoader.sourceComponent: DssH3{
                text: "Scanning..."
            }
        }
    }

    Timer {
        id: scanTimer
        running: wirelessDevicesExpand.expanded
        interval: 1000
        onTriggered: {
            print("Scan Start:", dev[0])
            wirelessDevicesExpand.accessPoints = nm.GetAccessPoints(dev[0])
            print("Scan Stop:", dev[0])
        }
    }
}
