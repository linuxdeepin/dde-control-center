import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "widgets"

Column {
    property int realHeight: childrenRect.height
    DBaseLine {
        leftLoader.sourceComponent: DssH1 {
            text: dsTr("Actived Connections")
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
    function getActiveConnectionInfo(devPath) {
        var c
        var connJSON = dbusNetwork.GetActiveConnectionInfo(devPath)
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
                    DssH2 { text: dsTr("DeviceType") }  DssH2 { text: getDeviceName(modelData.DeviceType) }
                    DssH2 { text: dsTr("DeviceAddr")}   DssH2 { text: modelData.HwAddress }
                    DssH2 { text: dsTr("IpAddress")}    DssH2 { text: modelData.IpAddress }
                    DssH2 { text: dsTr("SubnetMask")}   DssH2 { text: modelData.SubnetMask }
                    DssH2 { text: dsTr("RouteAddr")}    DssH2 { text: modelData.RouteAddress }
                    DssH2 { text: dsTr("Primary DNS")}  DssH2 { text: modelData.Dns1 }
                    DssH2 { text: dsTr("Addtional DNS"); visible: modelData.Dns2 != ""}  DssH2 { text: modelData.Dns2 ; visible: modelData.Dns2 != ""}
                    DssH2 { text: dsTr("DeviceSpeed")}  DssH2 { text: modelData.Speed + " Mb/s" }
                }
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseLine{
        rightLoader.sourceComponent: DTextButton{
            text: "Close"
            onClicked: {
                stackView.reset()
            }
        }
    }
}
