import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditPage {
    id: rootPage
    width: parent.width

    property var hotspotInfo
    property string devicePath: "/"

    connectionSession: {
        if(hotspotInfo){
            return editConnection(hotspotInfo.Uuid, devicePath)
        }
        else{
            return createConnection(nmConnectionTypeWirelessHotspot, devicePath)
        }
    }

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: rootPage.hotspotInfo ? dsTr("Edit Wi-Fi Hotspot") :dsTr("Create Wi-Fi Hotspot")
        }
    }
    DSeparatorHorizontal {}

    // copy from ../edit_autogen/EditSectionWifi.qml
    EditLineTextInput {
        id: line80211WirelessSsid
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "802-11-wireless"
        key: "ssid"
        text: dsTr("SSID")
    }
    
    // copy from ../edit_autogen/EditSectionSecurity.qml
    EditLineComboBox {
        id: line80211WirelessSecurityVkKeyMgmt
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "802-11-wireless-security"
        key: "vk-key-mgmt"
        text: dsTr("Security")
    }
    EditLinePasswordInput {
        id: line80211WirelessSecurityWepKey0
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "802-11-wireless-security"
        key: "wep-key0"
        text: dsTr("Key")
    }
    EditLinePasswordInput {
        id: line80211WirelessSecurityPsk
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "802-11-wireless-security"
        key: "psk"
        text: dsTr("Password")
    }
    
    DSeparatorHorizontal {}
    
    // TODO
    Row {
        height: 38
        spacing: 10
        anchors.right: parent.right
        anchors.rightMargin: 15

        DTextButton {
            text: dsTr("Cancel")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                stackView.reset()
                connectionSession.Close()
            }
        }

        DTextButton {
            text: hotspotInfo ? dsTr("Save") : dsTr("Create")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                rootPage.generalSetKey("connection", "id", line80211WirelessSsid.getKey())
                rootPage.checkKeysInPage()
                if (connectionSession.Save()) {
                    stackView.reset()
                } else {
                    rootPage.showErrors()
                }
            }
        }
    }
}
