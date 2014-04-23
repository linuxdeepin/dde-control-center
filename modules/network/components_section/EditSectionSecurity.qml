import QtQuick 2.1
import Deepin.Widgets 1.0
import "../components"

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
            id: lineWepkey0
            key: "wep-key0"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: lineWepkey0
        }
        EditLinePassword{
            id: linePsk
            key: "psk"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: linePsk
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
            nameFilters: ["DER or PEM certificates(*.der *.pem *.crt *.cer)"] //TODO
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
            nameFilters: ["PAC files(*.pac)"] //TODO
        }
        EditLineFileChooser{
            key: "phase2-private-key"
            text: dsTr("phase2-private-key")// TODO
            nameFilters: ["DER, PEM or PKCS#12 private key(*.der *.pem *.p12 *.key)"] //TODO
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
            id: linePassword
            key: "password"
            text: dsTr("password")// TODO
        }
        EditLinePasswordToggle{
            targetTextInput: linePassword
        }
    }
}
