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
        EditLinePasswordInput{
            id: lineWepkey0
            key: "wep-key0"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: lineWepkey0
        }
        EditLinePasswordInput{
            id: linePsk
            key: "psk"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: linePsk
        }
        EditLineComboBox{
            key: "vk-eap"
            text: dsTr("EAP Auth")
        }
        EditLineTextInput{
            key: "anonymous-identity"
            text: dsTr("Anonymous Identity") // TODO
        }
        EditLineFileChooser {
            id: lineVkPacFile
            key: "vk-pac-file"
            text: dsTr("PAC file")
            nameFilters: ["PAC files(*.pac)"] //TODO
        }
        EditLineFileChooser {
            id: lineVkCaCert
            key: "vk-ca-cert"
            text: dsTr("CA Cert")
            nameFilters: ["DER or PEM certificates(*.der *.pem *.crt *.cer)"] //TODO
        }
        EditLineFileChooser {
            id: lineVkClientCert
            key: "vk-client-cert"
            text: dsTr("User Cert")
        }
        EditLineFileChooser {
            id: lineVkPrivateKey
            key: "vk-private-key"
            text: dsTr("Private Key")
            nameFilters: ["DER, PEM or PKCS#12 private key(*.der *.pem *.p12 *.key)"] //TODO
        }
        EditLineComboBox{
            key: "phase1-fast-provisioning"
            text: dsTr("PAC Provisioning") //TODO
        }
        EditLineComboBox{
            key: "phase1-peapver"
            text: dsTr("PEAP Version")
        }
        EditLineComboBox{
            key: "phase2-auth"
            text: dsTr("Inner Auth")
            alwaysUpdate: true
        }
        EditLineTextInput{
            key: "identity"
            text: dsTr("Identity")
        }
        EditLinePasswordInput{
            id: linePassword
            key: "password"
            text: dsTr("Password")
        }
        EditLinePasswordToggle{
            targetTextInput: linePassword
        }
        EditLinePasswordInput {
            id: linePrivateKeyPassword
            key: "private-key-password"
            text: dsTr("Private Password") //TODO
        }
        EditLinePasswordToggle{
            targetTextInput: linePrivateKeyPassword
        }
    }
}
