import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"

Column {
    id: rootPage
    width: parent.width
    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    property var connectionSessionObject: {
        var connectionPath = dbusNetwork.CreateConnection(nmConnectionTypeWireless, "/")
        return connectionSession.createObject(rootPage, { path: connectionPath })
    }

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Connect to hidden access point")
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

        PropertyLine{
            id: apSecure
            title.text: dsTr("Security")
            section: "security"
            key: "vk-key-mgmt"

            property var availableKeys: new Array()
            property var availableValues: new Array()

            function initSecurity(){
                var d = unmarshalJSON(connectionSessionObject.GetAvailableValues(section, key))
                for(var i in d){
                    if(d[i].Value != "wpa-eap"){
                        availableKeys.push(d[i].Value)
                        availableValues.push(d[i].Text)
                    }
                }
            }


            content.sourceComponent: DComboBox {
                id: securityCombobox
                width: settingBox.rightWidth

                Component.onCompleted: {
                    apSecure.initSecurity()
                    text = apSecure.availableValues[selectIndex]
                }

                property int selectIndex: 1

                function menuSelect(i){
                    selectIndex = i
                    text = apSecure.availableValues[selectIndex]
                }

                onClicked: {
                    if(!rootMenu.visible){
                        var pos = mapToItem(null, 0, 0)
                        rootMenu.labels = apSecure.availableValues
                        rootMenu.requestMenuItem = securityCombobox
                        rootMenu.currentIndex = selectIndex
                        rootMenu.posX = pos.x
                        rootMenu.posY = pos.y + height
                        rootMenu.innerWidth = width
                    }
                    rootMenu.visible = !rootMenu.visible
                }
            }

            function getValue(){
                return apSecure.availableKeys[content.item.selectIndex]
            }

            function checkKey(){
                return true
            }
        }

        PropertyLine {
            id: apPassword
            title.text: dsTr("Password")
            section: "security"
            key: "psk"
            visible: apSecure.getValue() != 0

            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: settingBox.rightWidth
                echoMode: TextInput.Password
            }

            function getValue(){
                return content.item.text
            }

            function checkKey(){
                return true
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
            onClicked: stackView.reset()
        }

        DTextButton {
            text: dsTr("Connect")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(checkValid()){
                    connectHiddenAp()
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

    function connectHiddenAp(){
        for(var i=0; i<settingBox.children.length; i++){
            var objLine = settingBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                setValue(objLine.section, objLine.key, objLine.getValue())
            }
        }
        setValue("general", "id", apSsid.getValue())
        if(!connectionSessionObject.Save()){
            print("Connect to hidden AP error:", connectionSessionObject.errors)
            connectionSessionObject.Close()
        }
        stackView.reset()
    }

    function setValue(section, key, value){
        connectionSessionObject.SetKey(section, key, marshalJSON(value))
    }

}
