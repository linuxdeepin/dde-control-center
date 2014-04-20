import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

BaseEditSection {
    id: ipv4Settings
    sectionName: "IPv4"

    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("IPv4")
    }

    content.sourceComponent: Column {
        EditLineComboBox {
            section: ipv4Settings.sectionName
            key: "method"
            text: dsTr("Method")
        }
        EditLineIpv4 {
            section: ipv4Settings.sectionName
            key: "vk-addresses-address"
            text: dsTr("IP Address")
        }
        EditLineIpv4 {
            section: ipv4Settings.sectionName
            key: "vk-addresses-mask"
            text: dsTr("Netmask")
        }
        EditLineIpv4 {
            section: ipv4Settings.sectionName
            key: "vk-addresses-gateway"
            text: dsTr("Gateway")
        }
        EditLineIpv4 {
            section: ipv4Settings.sectionName
            key: "vk-dns"
            text: dsTr("DNS Server")
        }
    }
}

