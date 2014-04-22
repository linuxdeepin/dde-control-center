import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditSection{
    id: sectionSecurity
    section: "security"

    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("Security")
    }

    content.sourceComponent: Column {
        EditLineComboBox{
            key: "vk-key-mgmt"
            text: dsTr("Security:") // TODO section name append with ":"
        }
        EditLinePassword{
            id: wepkey0
            key: "wep-key0"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: wepkey0
        }
        EditLinePassword{
            id: psk
            key: "psk"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: psk
        }
        EditLineComboBox{
            key: "vk-eap"
            text: dsTr("vk-eap")// TODO
        }
        EditLineTextInput{
            key: "anonymous-identity"
            text: dsTr("anonymous-identity")// TODO
        }
        EditLineFileChooser{
            key: "ca-cert"
            text: dsTr("ca-cert")// TODO
            nameFilters: ["(*.png *.jpg *.bmp)"] //TODO
        }
        EditLineFileChooser{
            key: "client-cert"
            text: dsTr("client-cert")// TODO
        }
        EditLineComboBox{
            key: "phase1-fast-provisioning"
            text: dsTr("phase1-fast-provisioning")// TODO
        }
        EditLineFileChooser{
            key: "pac-file"
            text: dsTr("pac-file")// TODO
        }
        EditLineFileChooser{
            key: "phase2-private-key"
            text: dsTr("phase2-private-key")// TODO
        }
        EditLineComboBox{
            key: "phase1-peapver"
            text: dsTr("phase1-peapver")// TODO
        }
        EditLineComboBox{
            key: "phase2-auth"
            text: dsTr("phase2-auth")// TODO
            alwaysUpdate: true
        }
        EditLineTextInput{
            key: "identity"
            text: dsTr("identity")// TODO
        }
        EditLinePassword{
            id: password
            key: "password"
            text: dsTr("password")// TODO
        }
        EditLinePasswordToggle{
            targetTextInput: password
        }
    }
}
