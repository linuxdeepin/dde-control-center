import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "widgets"

Column {
    DBaseLine {
        leftLoader.sourceComponent: DssH1 {
            text: dsTr("Actived Connections")
        }
    }
    DSeparatorHorizontal{}

    function filterConnection(devs) {
        var conns = []
        for (var i=0; i<devs.length; i++) {
            var c = dbusNetwork.GetActiveConnection(devs[i][0])
            if (c && c[0]) {
                conns.push(c)
            }
        }
        return conns
    }

    Repeater {
        model: filterConnection(dbusNetwork.wiredDevices)
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
                    DssH2 { text: dsTr("DeviceType") }         DssH2 { text: dsTr("wired") }
                    DssH2 { text: dsTr("DeviceAddr")}      DssH2 { text: modelData[1] }
                    DssH2 { text: dsTr("IpAddress")}        DssH2 { text: modelData[2] }
                    DssH2 { text: dsTr("SubnetMask")}      DssH2 { text: modelData[3] }
                    DssH2 { text: dsTr("RouteAddr")}      DssH2 { text: modelData[4] }
                    DssH2 { text: dsTr("DeviceSpeed")}      DssH2 { text: modelData[5] + "Mb/s" }
                }
            }
        }
    }

    Repeater {
        model: filterConnection(dbusNetwork.wirelessDevices)
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
                    DssH2 { text: dsTr("DeviceType") }         DssH2 { text: dsTr("wireless") }
                    DssH2 { text: dsTr("DeviceAddr")}      DssH2 { text: modelData[1] }
                    DssH2 { text: dsTr("IpAddress")}        DssH2 { text: modelData[2] }
                    DssH2 { text: dsTr("SubnetMask")}      DssH2 { text: modelData[3] }
                    DssH2 { text: dsTr("RouteAddr")}      DssH2 { text: modelData[4] }
                    DssH2 { text: dsTr("DeviceSpeed")}      DssH2 { text: modelData[5] + "Mb/s" }
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
