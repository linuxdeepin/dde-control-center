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
            text: rootPage.hotspotInfo ? dsTr("Edit Wifi Hotspot") :dsTr("Create Wifi Hotspot")
        }
    }
    DSeparatorHorizontal {}

    // copy from ../edit_autogen/EditSectionWifi.qml
    EditLineTextInput {
        id: line80211WirelessSsid
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
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
        connectionData: rootPage.connectionData
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
        connectionData: rootPage.connectionData
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
        connectionData: rootPage.connectionData
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
                generalSetKey("connection", "id", line80211WirelessSsid.getKey())
                checkKeysInPage()
                if (connectionSession.Save()) {
                    stackView.reset()
                }
            }
        }
    }

    // TODO remove
    // Column {
    //     id: settingBox
    //     width: parent.width

    //     property int edgePadding: 24
    //     property int leftWidth: edgePadding
    //     property int rightWidth: settingBox.width - leftWidth - edgePadding

    //     function updateLeftWidth(newWidth){
    //         if(newWidth + edgePadding > settingBox.leftWidth){
    //             settingBox.leftWidth = newWidth + edgePadding
    //         }
    //     }

    //     PropertyLine {
    //         id: apSsid
    //         title.text: dsTr("SSID")
    //         section: "wifi"
    //         key: "ssid"
    //         content.sourceComponent: DTextInput {
    //             activeFocusOnTab: true
    //             width: settingBox.rightWidth

    //             Component.onCompleted: {
    //                 text = getKey(apSsid.section, apSsid.key)
    //             }
    //         }

    //         function getValue(){
    //             return content.item.text
    //         }

    //         function checkKey(){
    //             var valid = content.item.text != ""
    //             if(!valid){
    //                 content.item.state = "warning"
    //             }
    //             return valid
    //         }
    //     }

    //     PropertyLine {
    //         id: apPassword
    //         title.text: dsTr("Password")
    //         section: "security"
    //         key: "wep-key0"

    //         content.sourceComponent: DTextInput{
    //             activeFocusOnTab: true
    //             width: settingBox.rightWidth
    //             echoMode: TextInput.Password
    //             Component.onCompleted: {
    //                 text = getKey(apPassword.section, apPassword.key)
    //             }
    //         }

    //         function getValue(){
    //             return content.item.text
    //         }

    //         function checkKey(){
    //             // WEP password
    //             var valid = content.item.text.length >= 8
    //             if(!valid){
    //                 content.item.state = "warning"
    //             }
    //             return valid
    //         }
    //     }

    // }


    // function checkValid(){
    //     var valid = true
    //     for(var i=0; i<settingBox.children.length; i++){
    //         var objLine = settingBox.children[i]
    //         if(objLine.objectName == "PropertyLine" && objLine.visible){
    //             valid = objLine.checkKey() && valid
    //         }
    //     }
    //     return valid
    // }

    // function createWifiHotspot(){
    //     setValue("general", "id", apSsid.getValue())
    //     setValue("security", "vk-key-mgmt", "wep")
    //     for(var i=0; i<settingBox.children.length; i++){
    //         var objLine = settingBox.children[i]
    //         if(objLine.objectName == "PropertyLine" && objLine.visible){
    //             setValue(objLine.section, objLine.key, objLine.getValue())
    //         }
    //     }
    //     if(!connectionSession.Save()){
    //         print("create Wifi hotspot error:", connectionSession.errors)
    //         connectionSession.Close()
    //     }
    //     stackView.reset()
    // }

    // function setValue(section, key, value){
    //     connectionSession.SetKey(section, key, marshalJSON(value))
    // }
    // function getKey(section, key){
    //     var value = connectionSession.GetKey(section, key)
    //     return unmarshalJSON(value)
    // }
}
