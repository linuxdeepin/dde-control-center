import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"

Column {
    id: rootPage
    width: parent.width
    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("New VPN connection")
        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        leftLoader.sourceComponent: DssH3{
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

            property string selectItemId: nmConnectionTypeVpnL2tp
            model: ListModel {}

            Component.onCompleted: {
                model.append({
                    "item_id": nmConnectionTypeVpnL2tp,
                    "item_name": dsTr("L2TP")
                });
                model.append({
                    "item_id": nmConnectionTypeVpnPptp,
                    "item_name": dsTr("PPTP")
                });
                //model.append({
                    //"item_id": nmConnectionTypeVpnVpnc,
                    //"item_name": dsTr("VPNC")
                //});
                //model.append({
                    //"item_id": nmConnectionTypeVpnOpenvpn,
                    //"item_name": dsTr("OPENVPN")
                //});
                //model.append({
                    //"item_id": nmConnectionTypeVpnOpenconnect,
                    //"item_name": dsTr("OpenVPNConnect")
                //});
            }

            delegate: SelectItem {
                labelLeftMargin: itemLabelLeftMargin
                totalItemNumber: typeList.count
                selectItemId: typeList.selectItemId

                onSelectAction: {
                    typeList.selectItemId = itemId
                }
            }
        } // End of typeList
    }

    DSeparatorHorizontal {}

    DBaseLine {
        leftLoader.sourceComponent: DssH3 {
            text: dsTr("Basic Information")
        }
    }
    DSeparatorHorizontal {}

    Column {
        id: addVpnBox
        width: parent.width
        property string selectVpnType: typeList.selectItemId

        property int edgePadding: 24
        property int leftWidth: edgePadding
        property int rightWidth: addVpnBox.width - leftWidth - edgePadding

        function updateLeftWidth(newWidth){
            if(newWidth + edgePadding > addVpnBox.leftWidth){
                addVpnBox.leftWidth = newWidth + edgePadding
            }
        }

        PropertyLine {
            id: vpnName
            title.text: dsTr("Name")
            section: "general"
            key: "id"
            content.sourceComponent: DTextInput {
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
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
            section: typeList.selectItemId
            key: "gateway"
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
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
            section: typeList.selectItemId
            key: "user"
            visibleType: [nmConnectionTypeVpnL2tp, nmConnectionTypeVpnPptp, nmConnectionTypeVpnVpnc]
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
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
            visibleType: [nmConnectionTypeVpnL2tp, nmConnectionTypeVpnPptp]

            property bool active: false
            content.sourceComponent: Item {
                width: addVpnBox.rightWidth
                height: childrenRect.height
                DSwitchButton{
                    anchors.right: parent.right
                    onClicked: rsaSecurID.active = checked
                }
            }
            Component.onCompleted: {
                addVpnBox.updateLeftWidth(title.contentWidth)
            }
            function checkKey(){
                return true
            }
        }

        PropertyLine {
            id: vpnPassword
            title.text: dsTr("Password")
            section: typeList.selectItemId
            key: "password"
            visible: !rsaSecurID.active

            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
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

    DSeparatorHorizontal {}

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
            text: dsTr("Add")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(checkValid()){
                    createVpn()
                }
            }
        }
    }

    function checkValid(){
        var valid = true
        for(var i=0; i<addVpnBox.children.length; i++){
            var objLine = addVpnBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                valid = objLine.checkKey() && valid
            }
        }
        return valid
    }

    function createVpn(){
        var connectionPath = dbusNetwork.CreateConnection(typeList.selectItemId, "/")
        var sessionObject = connectionSession.createObject(rootPage, { path: connectionPath })
        for(var i=0; i<addVpnBox.children.length; i++){
            var objLine = addVpnBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                if(objLine.key){
                    sessionObject.SetKey(objLine.section, objLine.key, marshalJSON(objLine.getValue()))
                }
            }
        }
        var ok = true
        for(var i in sessionObject.availablePages){
            var pageName = sessionObject.availablePages[i]
            ok = !sessionObject.errors[pageName] && ok
        }
        if(!sessionObject.Save()){
            for(var i in sessionObject.availablePages){
                var pageName = sessionObject.availablePages[i]
                var error_info = sessionObject.errors[pageName]
                for(var key in error_info){
                    print("Error:", pageName, key, error_info[key])
                }
            }
        }else{
            stackView.reset()
        }
    }

    function goToCreateConnection(type){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "create": true, "type":  type},
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }

}
