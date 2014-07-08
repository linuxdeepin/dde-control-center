import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "widgets"

Column {
    property int realHeight: childrenRect.height
    DBaseLine {
        leftLoader.sourceComponent: DssH1 {
            text: {
                if (getActiveConnectionInfo()) {
                    return dsTr("Activated Connections")
                }
                return dsTr("None Activated Connection")
            }
        }
    }
    DSeparatorHorizontal{}

    function getActiveConnectionInfo() {
        var conns = []
        var json = dbusNetwork.GetActiveConnectionInfo()
        if (json) {
            conns = unmarshalJSON(json)
        }
        return conns
    }

    Repeater {
        model: getActiveConnectionInfo()
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
                    InfoItem { text: dsTr("Device Addr") }  InfoItem { text: modelData.HwAddress }
                    InfoItem { text: dsTr("IP Address") }   InfoItem { text: modelData.Ip4.Address }
                    InfoItem { text: dsTr("Subnet Mask") }  InfoItem { text: modelData.Ip4.Mask }
                    InfoItem { text: dsTr("Route Addr") }   InfoItem { text: modelData.Ip4.Router }
                    InfoItem { text: dsTr("Primary DNS") }  InfoItem { text: modelData.Ip4.Dns1 }
                    InfoItem { text: dsTr("Addtional DNS"); visible: modelData.Ip4.Dns2 != "" }  InfoItem { text: modelData.Ip4.Dns2 ; visible: modelData.Ip4.Dns2 != ""}
                    InfoItem { text: dsTr("Device Speed") } InfoItem { text: modelData.Speed + " Mb/s" }
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
