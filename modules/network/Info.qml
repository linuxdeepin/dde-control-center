import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column {
    DSeparatorHorizontal{}
    DBaseLine {
        leftLoader.sourceComponent: DssH1 {
            text: dsTr("Actived Connections")
        }
    }
    DSeparatorHorizontal{}

    function filterConnection(devs) {
        var conns = []
        for (var i=0; i<devs.length; i++) {
            var c = nm.GetActiveConnection(devs[i][0])
            if (c[0]) {
                conns.push(c)
            }
        }
        return conns
    }

    Repeater {
        model: filterConnection(nm.wiredDevices)
        DBaseExpand {
            width: root.width
            expanded: header.item.active
            header.sourceComponent: DDownArrowHeader {
                active: true
                text: modelData[0]
            }
            content.sourceComponent: DBaseLine {
                height: leftLoader.item.height
                color: dconstants.contentBgColor
                leftLoader.sourceComponent: GridLayout {
                    width: root.width
                    columns: 2
                    Layout.preferredWidth: root.width
                    DLabel { text: dsTr("DeviceType") }         DLabel { text: dsTr("wired") }
                    DLabel { text: dsTr("DeviceAddr")}      DLabel { text: modelData[1] }
                    DLabel { text: dsTr("IpAddress")}        DLabel { text: modelData[2] }
                    DLabel { text: dsTr("SubnetMask")}      DLabel { text: modelData[3] }
                    DLabel { text: dsTr("RouteAddr")}      DLabel { text: modelData[4] }
                    DLabel { text: dsTr("DeviceSpeed")}      DLabel { text: modelData[5] + "Mb/s" }
                    DLabel { text: dsTr("InternetSpeed")}      Button { text: dsTr("test it") }
                }
            }
        }
    }
    Repeater {
        model: filterConnection(nm.wirelessDevices)
        DBaseExpand {
            visible: true
            width: root.width
            expanded: header.item.active
            header.sourceComponent: DDownArrowHeader {
                active: true
                text: modelData[0]
            }
            content.sourceComponent: DBaseLine {
                height: leftLoader.item.height
                color: dconstants.contentBgColor
                leftLoader.sourceComponent: GridLayout {
                    width: root.width
                    columns: 2
                    Layout.preferredWidth: root.width
                    DLabel { text: dsTr("DeviceType") }         DLabel { text: dsTr("wireless") }
                    DLabel { text: dsTr("DeviceAddr")}      DLabel { text: modelData[1] }
                    DLabel { text: dsTr("IpAddress")}        DLabel { text: modelData[2] }
                    DLabel { text: dsTr("SubnetMask")}      DLabel { text: modelData[3] }
                    DLabel { text: dsTr("RouteAddr")}      DLabel { text: modelData[4] }
                    DLabel { text: dsTr("DeviceSpeed")}      DLabel { text: modelData[5] + "Mb/s" }
                    DLabel { text: dsTr("InternetSpeed")}      Button { text: dsTr("test it") }
                }
            }
        }
    }
}
