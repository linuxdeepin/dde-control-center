import QtQuick 2.1
import Deepin.Widgets 1.0
import "../../../shared"
import "../../widgets"

Column {
    id: settingBox
    width: parent.width

    property string vpnType: nmConnectionTypeVpnL2tp

    property int edgePadding: 24
    property int leftWidth: edgePadding
    property int rightWidth: settingBox.width - leftWidth - edgePadding

    function setDefaultKeys(sessionObject){
        if(vpnSecretKey.getValue()){
            sessionObject.SetKey("vpn-l2tp-ipsec", "ipsec-enabled", marshalJSON(true))
        }
    }

    function updateLeftWidth(newWidth){
        if(newWidth + edgePadding > settingBox.leftWidth){
            settingBox.leftWidth = newWidth + edgePadding
        }
    }

    PropertyLine {
        id: vpnName
        title.text: dsTr("Name")
        section: "connection"
        key: "id"
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

    PropertyLine {
        id: vpnServer
        title.text: dsTr("Server")
        section: "alias-vpn-l2tp"
        key: "gateway"
        content.sourceComponent: DTextInput{
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

    PropertyLine {
        id: vpnUserName
        title.text: dsTr("Account")
        section: "alias-vpn-l2tp"
        key: "user"
        content.sourceComponent: DTextInput{
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

    PropertyLine {
        id: rsaSecurID
        title.text: dsTr("RSA SecurID")

        property bool active: false
        content.sourceComponent: Item {
            width: settingBox.rightWidth
            height: childrenRect.height
            DSwitchButton{
                anchors.right: parent.right
                onClicked: rsaSecurID.active = checked
            }
        }
        Component.onCompleted: {
            settingBox.updateLeftWidth(title.contentWidth)
        }
        function checkKey(){
            return true
        }
    }

    PropertyLine {
        id: vpnPassword
        title.text: dsTr("Password")
        section: "alias-vpn-l2tp"
        key: "password"
        visible: !rsaSecurID.active

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

    PropertyLine {
        id: requireMppeLine
        title.text: dsTr("Enable MPPE")
        section: "vpn-l2tp-ppp"
        key: "require-mppe"

        property bool active: true
        content.sourceComponent: Item {
            width: settingBox.rightWidth
            height: childrenRect.height
            DSwitchButton{
                id: requireMppeSwitcher
                anchors.right: parent.right
                onClicked: requireMppeLine.active = checked
                Component.onCompleted: checked = requireMppeLine.active
            }
        }

        Component.onCompleted: {
            settingBox.updateLeftWidth(title.contentWidth)
        }

        function getValue(){
            return requireMppeLine.active
        }
        function checkKey(){
            return true
        }
    }

    PropertyLine {
        id: vpnSecretKey
        title.text: dsTr("Secret key")
        section: "vpn-l2tp-ipsec"
        key: "ipsec-psk"

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
