import QtQuick 2.1
import Deepin.Widgets 1.0
import "../../shared"
import "../edit"

BaseEditPage {
    id: rootPage
    width: parent.width

    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    DBaseLine {
        leftLoader.sourceComponent: DssH2{
            text: dsTr("VPN type")
        }
    }
    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: childrenRect.height
        color: dconstants.contentBgColor

        ListView {
            id: typeList
            width: parent.width
            height: childrenRect.height
            visible: count != 0

            property string selectVpnType: nmConnectionTypeVpnL2tp
            model: ListModel {}

            property var vpnNames: {
                var names = {}
                names[nmConnectionTypeVpnL2tp] = dsTr("L2TP")
                names[nmConnectionTypeVpnPptp] = dsTr("PPTP")
                names[nmConnectionTypeVpnOpenvpn] = dsTr("OpenVPN")
                names[nmConnectionTypeVpnOpenconnect] = dsTr("OpenConnect")
                names[nmConnectionTypeVpnVpnc] = dsTr("VPNC")
                return names
            }

            Component.onCompleted: {
                for(var key in vpnNames){
                    model.append({
                        "item_id": key,
                        "item_name": vpnNames[key]
                    })
                }
            }

            delegate: SelectItem {
                labelLeftMargin: itemLabelLeftMargin
                totalItemNumber: typeList.count
                selectItemId: typeList.selectVpnType

                onSelectAction: {
                    typeList.selectVpnType = itemId
                    rootPage.generalSetKey("vs-vpn", "vk-vpn-type", itemId)
                }
            }
        } // End of typeList
    }

    DSeparatorHorizontal {}

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Basic Information")
        }
    }
    DSeparatorHorizontal {}
    
    EditLineMissingPackage {
        id: lineVsVpnVkVpnMissingPlugin
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "vs-vpn"
        key: "vk-vpn-missing-plugin"
        text: dsTr("Need VPN Plugin")
        missingTypeName: typeList.vpnNames[typeList.selectVpnType]
    }

    DSeparatorHorizontal {
        visible: lineVsVpnVkVpnMissingPlugin.visible
    }

    Column {
        width: parent.width
        visible: !lineVsVpnVkVpnMissingPlugin.visible

        EditLineTextInput {
            id: lineConnectionId
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "connection"
            key: "id"
            text: dsTr("Name")
        }
        
        // copy from ../edit_autogen/EditSectionVpn.qml
        EditLineTextInput {
            id: lineAliasVpnL2tpGateway
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp"
            key: "gateway"
            text: dsTr("Gateway")
        }
        EditLineTextInput {
            id: lineAliasVpnL2tpUser
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp"
            key: "user"
            text: dsTr("Username")
        }
        EditLineComboBox {
            id: lineAliasVpnL2tpPasswordFlags
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp"
            key: "password-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnL2tpPassword
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp"
            key: "password"
            text: dsTr("Password")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenconnectGateway
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "gateway"
            text: dsTr("Gateway")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenconnectCacert
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "cacert"
            text: dsTr("CA Cert")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenconnectProxy
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "proxy"
            text: dsTr("Proxy")
        }
        EditLineSwitchButton {
            id: lineAliasVpnOpenconnectEnableCsdTrojan
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "enable_csd_trojan"
            text: dsTr("Allow Cisco Secure Desktop Trojan")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenconnectCsdWrapper
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "csd_wrapper"
            text: dsTr("CSD Wrapper")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenconnectUsercert
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "usercert"
            text: dsTr("Uesr Cert")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenconnectUserkey
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "userkey"
            text: dsTr("Private Key")
        }
        EditLineSwitchButton {
            id: lineAliasVpnOpenconnectPemPassphraseFsid
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openconnect"
            key: "pem_passphrase_fsid"
            text: dsTr("User FSID For Key Passphrase")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenvpnRemote
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "remote"
            text: dsTr("Gateway")
        }
        EditLineComboBox {
            id: lineAliasVpnOpenvpnConnectionType
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "connection-type"
            text: dsTr("Auth Type")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenvpnUsername
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "username"
            text: dsTr("Username")
        }
        EditLineComboBox {
            id: lineAliasVpnOpenvpnPasswordFlags
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "password-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnOpenvpnPassword
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "password"
            text: dsTr("Password")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenvpnCert
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "cert"
            text: dsTr("User Cert")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenvpnCa
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "ca"
            text: dsTr("CA Cert")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenvpnKey
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "key"
            text: dsTr("Private Key")
        }
        EditLineComboBox {
            id: lineAliasVpnOpenvpnCertPassFlags
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "cert-pass-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnOpenvpnCertPass
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "cert-pass"
            text: dsTr("Private Pwd")
        }
        EditLineFileChooser {
            id: lineAliasVpnOpenvpnStaticKey
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "static-key"
            text: dsTr("Static Key")
        }
        EditLineSwitchButton {
            id: lineAliasVpnOpenvpnVkStaticKeyDirection
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "vk-static-key-direction"
            text: dsTr("Use Key Direction")
        }
        EditLineComboBox {
            id: lineAliasVpnOpenvpnStaticKeyDirection
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "static-key-direction"
            text: dsTr("Key Direction")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenvpnRemoteIp
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "remote-ip"
            text: dsTr("Remote IP")
        }
        EditLineTextInput {
            id: lineAliasVpnOpenvpnLocalIp
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-openvpn"
            key: "local-ip"
            text: dsTr("Local IP")
        }
        EditLineTextInput {
            id: lineAliasVpnPptpGateway
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-pptp"
            key: "gateway"
            text: dsTr("Gateway")
        }
        EditLineTextInput {
            id: lineAliasVpnPptpUser
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-pptp"
            key: "user"
            text: dsTr("Username")
        }
        EditLineComboBox {
            id: lineAliasVpnPptpPasswordFlags
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-pptp"
            key: "password-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnPptpPassword
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-pptp"
            key: "password"
            text: dsTr("Password")
        }
        EditLineTextInput {
            id: lineAliasVpnVpncIpsecGateway
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "IPSec gateway"
            text: dsTr("Gateway")
        }
        EditLineTextInput {
            id: lineAliasVpnVpncXauthUsername
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "Xauth username"
            text: dsTr("Username")
        }
        EditLineComboBox {
            id: lineAliasVpnVpncXauthPasswordFlags
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "Xauth password-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnVpncXauthPassword
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "Xauth password"
            text: dsTr("User Pwd")
        }
        EditLineTextInput {
            id: lineAliasVpnVpncIpsecId
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "IPSec ID"
            text: dsTr("Group Name")
        }
        EditLineComboBox {
            id: lineAliasVpnVpncIpsecSecretFlags
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "IPSec secret-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnVpncIpsecSecret
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "IPSec secret"
            text: dsTr("Group Pwd")
        }
        EditLineSwitchButton {
            id: lineAliasVpnVpncVkHybridAuthmode
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "vk-hybrid-authmode"
            text: dsTr("Use Hybrid Authentication")
        }
        EditLineFileChooser {
            id: lineAliasVpnVpncCaFile
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-vpnc"
            key: "CA-File"
            text: dsTr("CA File")
        }
        
        // copy from ../edit_autogen/EditSectionVpnL2tpIpsec.qml
        EditLineSwitchButton {
            id: lineAliasVpnL2tpIpsecIpsecEnabled
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp-ipsec"
            key: "ipsec-enabled"
            text: dsTr("Enable IPsec")
        }
        EditLineTextInput {
            id: lineAliasVpnL2tpIpsecIpsecGroupName
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp-ipsec"
            key: "ipsec-group-name"
            text: dsTr("Group Name")
        }
        EditLineTextInput {
            id: lineAliasVpnL2tpIpsecIpsecGatewayId
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp-ipsec"
            key: "ipsec-gateway-id"
            text: dsTr("Group ID")
        }
        EditLinePasswordInput {
            id: lineAliasVpnL2tpIpsecIpsecPsk
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp-ipsec"
            key: "ipsec-psk"
            text: dsTr("Password")
        }
        
        // copy from ../edit_autogen/EditSectionVpnL2tpPpp.qml
        EditLineSwitchButton {
            id: lineAliasVpnL2tpPppVkRequireMppe
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-l2tp-ppp"
            key: "vk-require-mppe"
            text: dsTr("Use MPPE")
        }
        
        // copy from ../edit_autogen/EditSectionVpnPptpPpp.qml
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppVkRequireMppe
            connectionSession: rootPage.connectionSession
            availableSections: rootPage.availableSections
            availableKeys: rootPage.availableKeys
            connectionData: rootPage.connectionData
            errors: rootPage.errors
            section: "alias-vpn-pptp-ppp"
            key: "vk-require-mppe"
            text: dsTr("Use MPPE")
        }
        
        DSeparatorHorizontal {}

        AddSectionSave {}
    }
}
