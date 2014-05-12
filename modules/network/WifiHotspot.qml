import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"
import "widgets"

Column {
    id: rootPage
    width: parent.width

    property var hotspotInfo
    property string devicePath: "/"

    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    property var connectionSessionObject: {
        if(hotspotInfo){
            var connectionPath = dbusNetwork.EditConnection(hotspotInfo.Uuid, devicePath)
        }
        else{
            var connectionPath = dbusNetwork.CreateConnection(nmConnectionTypeWirelessAdhoc, "/")
        }
        return connectionSession.createObject(rootPage, { path: connectionPath })
    }

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: rootPage.hotspotInfo ? dsTr("Edit Wifi Hotspot") :dsTr("Create Wifi Hotspot")
        }
    }

    DSeparatorHorizontal {}

    Column {
        id: settingBox
        width: parent.width

        property int edgePadding: 24
        property int leftWidth: edgePadding
        property int rightWidth: settingBox.width - leftWidth - edgePadding

        function updateLeftWidth(newWidth){
            if(newWidth + edgePadding > settingBox.leftWidth){
                settingBox.leftWidth = newWidth + edgePadding
            }
        }

        PropertyLine {
            id: apSsid
            title.text: dsTr("SSID")
            section: "wifi"
            key: "ssid"
            content.sourceComponent: DTextInput {
                activeFocusOnTab: true
                width: settingBox.rightWidth

                Component.onCompleted: {
                    text = getKey(apSsid.section, apSsid.key)
                }
            }

            function getValue(){
                return content.item.text
            }

            function checkKey(){
                var valid = content.item.text != ""
                if(!valid){
                    content.item.state = "warning"
                }
                return valid
            }
        }

        PropertyLine {
            id: apPassword
            title.text: dsTr("Password")
            section: "security"
            key: "wep-key0"

            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: settingBox.rightWidth
                echoMode: TextInput.Password
                Component.onCompleted: {
                    text = getKey(apPassword.section, apPassword.key)
                }
            }

            function getValue(){
                return content.item.text
            }

            function checkKey(){
                // WEP password
                var valid = content.item.text.length >= 8
                if(!valid){
                    content.item.state = "warning"
                }
                return valid
            }
        }

    }

    Row {
        height: 38
        spacing: 10
        anchors.right: parent.right
        anchors.rightMargin: 15

        DTextButton {
            text: dsTr("Cancel")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                connectionSessionObject.Close()
                stackView.reset()
            }
        }

        DTextButton {
            text: hotspotInfo ? dsTr("Save") : dsTr("Create") 
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(checkValid()){
                    createWifiHotspot()
                }
            }
        }
    }

    function checkValid(){
        var valid = true
        for(var i=0; i<settingBox.children.length; i++){
            var objLine = settingBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                valid = objLine.checkKey() && valid
            }
        }
        return valid
    }

    function createWifiHotspot(){
        setValue("general", "id", apSsid.getValue())
        setValue("security", "vk-key-mgmt", "wep")
        for(var i=0; i<settingBox.children.length; i++){
            var objLine = settingBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                setValue(objLine.section, objLine.key, objLine.getValue())
            }
        }
        if(!connectionSessionObject.Save()){
            print("create Wifi hotspot error:", connectionSessionObject.errors)
            connectionSessionObject.Close()
        }
        stackView.reset()
    }

    function setValue(section, key, value){
        connectionSessionObject.SetKey(section, key, marshalJSON(value))
    }
    function getKey(section, key){
        var value = connectionSessionObject.GetKey(section, key)
        return unmarshalJSON(value)
    }
}
