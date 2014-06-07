import QtQuick 2.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: wiredDeviceExpand
    width: parent.width
    visible: wiredDevicesNumber > 0

    property var wiredDevices: nmDevices[nmDeviceTypeEthernet]
    property int wiredDevicesNumber: wiredDevices.length

    expanded: dbusNetwork.wiredEnabled

    header.sourceComponent: DBaseLine{
        id: wiredDeviceHeader
        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: dsTr("Wired Network")
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: wiredDeviceExpand.expanded
            onClicked: {
                dbusNetwork.wiredEnabled = checked
            }
        }
    }

    content.sourceComponent: Item {
        width: parent.width
        height: childrenRect.height

        ListView {
            width: parent.width
            height: childrenRect.height
            boundsBehavior: Flickable.StopAtBounds
            model: wiredDevicesNumber
            delegate: WiredItem {}
        }

    }
}
