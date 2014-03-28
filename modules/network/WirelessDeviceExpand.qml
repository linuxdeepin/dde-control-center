import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property variant dev
    property variant accessPoints : nm.GetAccessPoints(dev[0])

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
            width: parent.width
            height: childrenRect.height
            model: wirelessDevicesExpand.accessPoints
            delegate: WirelessItem {}
            Component.onCompleted: {
            }
        }

    }

    Timer {
        running: wirelessDevicesExpand.expanded
        interval: 1000
        onTriggered : {
            wirelessDevicesExpand.accessPoints = nm.GetAccessPoints(dev[0])
        }
    }
}
