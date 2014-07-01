import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "widgets"

Column {
    property int realHeight: childrenRect.height
    DBaseLine {
        leftLoader.sourceComponent: DssH1 {
            text: dsTr("Activated Connections")
        }
    }
    DSeparatorHorizontal{}

    function filterConnection() {
        var conns = []
        doFilterConnection(conns, nmDevices[nmDeviceTypeEthernet])
        doFilterConnection(conns, nmDevices[nmDeviceTypeWifi])
        return conns
    }
    function doFilterConnection(conns, devs) {
        for (var i in devs) {
            var c = getActiveConnectionInfo(devs[i].Path)
            if (c) {
                conns.push(c)
            }
        }
        return conns
    }
    function getActiveConnectionInfo(devicePath) {
        var c
        var connJSON = dbusNetwork.GetActiveConnectionInfo(devicePath)
        if (connJSON != "") {
            c = unmarshalJSON(connJSON)
        }
        return c
    }

    Repeater {
        model: filterConnection()
        DBaseExpand {
            width: root.width
            expanded: header.item.active
            header.sourceComponent: DDownArrowHeader {
                active: true
                text: modelData.Interface
            }
            content.sourceComponent: DBaseLine {
                height: leftLoader.item.height
                color: dconstants.contentBgColor
                leftLoader.sourceComponent: GridLayout {
                    width: root.width
                    columns: 2
                    Layout.preferredWidth: root.width
                    InfoItem { text: dsTr("Device Type") }  InfoItem { text: getDeviceName(modelData.DeviceType) }
                    InfoItem { text: dsTr("Device Addr")}   InfoItem { text: modelData.HwAddress }
                    InfoItem { text: dsTr("IP Address")}    InfoItem { text: modelData.IpAddress }
                    InfoItem { text: dsTr("Subnet Mask")}   InfoItem { text: modelData.SubnetMask }
                    InfoItem { text: dsTr("Route Addr")}    InfoItem { text: modelData.RouteAddress }
                    InfoItem { text: dsTr("Primary DNS")}  InfoItem { text: modelData.Dns1 }
                    InfoItem { text: dsTr("Addtional DNS"); visible: modelData.Dns2 != ""}  InfoItem { text: modelData.Dns2 ; visible: modelData.Dns2 != ""}
                    InfoItem { text: dsTr("Device Speed")}  InfoItem { text: modelData.Speed + " Mb/s" }
                }
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseLine{
        rightLoader.sourceComponent: DTextButton{
            text: dsTr("Close")
            onClicked: {
                stackView.reset()
            }
        }
    }
}
