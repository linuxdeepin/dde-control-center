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
                text: {
                    if (modelData.IsPrimaryConnection) {
                        return modelData.ConnectionName + dsTr("(default)")
                    }
                    return modelData.ConnectionName
                }
            }
            content.sourceComponent: DBaseLine {
                height: leftLoader.item.height
                color: dconstants.contentBgColor
                leftLoader.sourceComponent: Column {
                    width: root.width
                    InfoItem {
                        label: dsTr("Interface");
                        value: getConnectionDisplayName(modelData.ConnectionType) + " (" + modelData.DeviceInterface + ")"
                    }
                    InfoItem {label: dsTr("Security");       value: modelData.Security}
                    InfoItem {label: dsTr("Device Addr");    value: modelData.HwAddress}
                    InfoItem {label: dsTr("IP Address");     value: modelData.Ip4.Address}
                    InfoItem {label: dsTr("Subnet Mask");    value: modelData.Ip4.Mask}
                    InfoItem {label: dsTr("Route Addr");     value: modelData.Ip4.Route}
                    InfoItem {label: dsTr("Primary DNS");    value: modelData.Ip4.Dns1}
                    InfoItem {label: dsTr("Secondary DNS");  value: modelData.Ip4.Dns2}
                    InfoItem {label: dsTr("Third DNS");      value: modelData.Ip4.Dns3}

                    // TODO
                    // InfoItem {label: dsTr("IPv6 Address");     value: modelData.Ip6.Address}
                    // InfoItem {label: dsTr("IPv6 Route Addr");     value: modelData.Ip6.Route}
                    // InfoItem {label: dsTr("IPv6 Primary DNS");    value: modelData.Ip6.Dns1}
                    // InfoItem {label: dsTr("IPv6 Secondary DNS");  value: modelData.Ip6.Dns2}
                    // InfoItem {label: dsTr("IPv6 Ternary DNS");    value: modelData.Ip6.Dns3}

                    InfoItem {label: dsTr("Device Speed");   value: modelData.Speed}
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
