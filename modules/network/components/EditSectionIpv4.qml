import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

BaseEditSection {
    id: sectionIpv4
    section: "ipv4"

    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("IPv4")
    }

    content.sourceComponent: Column {
        EditLineComboBox {
            key: "method"
            text: dsTr("Method")
        }
        // TODO fix focus issue
        EditLineIpv4Input {
            key: "vk-addresses-address"
            text: dsTr("IP Address")
        }
        EditLineIpv4Input {
            key: "vk-addresses-mask"
            text: dsTr("Netmask")
        }
        EditLineIpv4Input {
            key: "vk-addresses-gateway"
            text: dsTr("Gateway")
        }
        EditLineIpv4Input {
            key: "vk-dns"
            text: dsTr("DNS Server")
        }
    }
}

