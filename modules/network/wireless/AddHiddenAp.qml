import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditPage {
    id: addPage
    width: parent.width

    Component.onCompleted: {
        line80211WirelessSecurityVkKeyMgmt.setKey("wpa-psk")
    }
    
    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Connect to hidden access point")
        }
    }
    DSeparatorHorizontal {}
    
    // copy from ../edit_autogen/EditSectionWifi.qml
    EditLineTextInput {
        id: line80211WirelessSsid
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-11-wireless"
        key: "ssid"
        text: dsTr("SSID")
    }
    
    // copy from ../edit_autogen/EditSectionSecurity.qml
    EditLineComboBox {
        id: line80211WirelessSecurityVkKeyMgmt
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-11-wireless-security"
        key: "vk-key-mgmt"
        text: dsTr("Security")
    }
    EditLinePasswordInput {
        id: line80211WirelessSecurityWepKey0
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-11-wireless-security"
        key: "wep-key0"
        text: dsTr("Key")
    }
    EditLinePasswordInput {
        id: line80211WirelessSecurityPsk
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-11-wireless-security"
        key: "psk"
        text: dsTr("Password")
    }
    EditLineComboBox {
        id: line8021xVkEap
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "vk-eap"
        text: dsTr("EAP Auth")
    }
    EditLineTextInput {
        id: line8021xIdentity
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "identity"
        text: dsTr("Identity")
    }
    EditLineTextInput {
        id: line8021xAnonymousIdentity
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "anonymous-identity"
        text: dsTr("Anonymous ID")
    }
    EditLineFileChooser {
        id: line8021xVkCaCert
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "vk-ca-cert"
        text: dsTr("CA Cert")
    }
    EditLineFileChooser {
        id: line8021xVkClientCert
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "vk-client-cert"
        text: dsTr("User Cert")
    }
    EditLineComboBox {
        id: line8021xPhase1FastProvisioning
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "phase1-fast-provisioning"
        text: dsTr("Provisioning")
    }
    EditLineFileChooser {
        id: line8021xVkPacFile
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "vk-pac-file"
        text: dsTr("PAC file")
    }
    EditLineComboBox {
        id: line8021xPhase1Peapver
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "phase1-peapver"
        text: dsTr("PEAP Version")
    }
    EditLineComboBox {
        id: line8021xPhase2Auth
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "phase2-auth"
        text: dsTr("Inner Auth")
        alwaysUpdate: true
    }
    EditLineFileChooser {
        id: line8021xVkPrivateKey
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "vk-private-key"
        text: dsTr("Private Key")
    }
    EditLineComboBox {
        id: line8021xPasswordFlags
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "password-flags"
        text: dsTr("Ask for Pwd")
        alwaysUpdate: true
    }
    EditLinePasswordInput {
        id: line8021xPassword
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "password"
        text: dsTr("Password")
    }
    EditLineComboBox {
        id: line8021xPrivateKeyPasswordFlags
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "private-key-password-flags"
        text: dsTr("Ask for Pwd")
    }
    EditLinePasswordInput {
        id: line8021xPrivateKeyPassword
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "802-1x"
        key: "private-key-password"
        text: dsTr("Private Pwd")
    }
    DSeparatorHorizontal {}
    
    Row {
        height: 38
        spacing: 10
        anchors.right: parent.right
        anchors.rightMargin: 15
        DTextButton {
            text: dsTr("Connect")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                generalSetKey("connection", "id", line80211WirelessSsid.getKey())
                checkKeysInPage()
                if (connectionSession.Save()) {
                    stackView.reset()
                }
            }
        }
        DTextButton {
            text: dsTr("Cancel")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                stackView.reset()
                connectionSession.Close()
            }
        }
    }
}
