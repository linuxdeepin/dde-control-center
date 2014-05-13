import QtQuick 2.1
import Deepin.Widgets 1.0
import "../../shared"
import "../widgets"

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

            property string selectVpnType: nmConnectionTypeVpnL2tp
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
                selectItemId: typeList.selectVpnType

                onSelectAction: {
                    typeList.selectVpnType = itemId
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
        id: vpnSettingBox
        width: parent.width

        function getSelectedBox(){
            for(var i=0; i<vpnSettingBox.children.length; i++){
                var box = vpnSettingBox.children[i]
                if(box.visible){
                    return box
                }
            }
        }

        SimplePptp {
            id: pptpBox
            visible: typeList.selectVpnType == vpnType
        }

        SimpleL2tp {
            id: l2tpBox
            visible: typeList.selectVpnType == vpnType
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
                var box = vpnSettingBox.getSelectedBox()
                if(checkValid(box)){
                    createVpn(box)
                }
            }
        }
    }

    function checkValid(box){
        var valid = true
        for(var i=0; i<box.children.length; i++){
            var objLine = box.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                valid = objLine.checkKey() && valid
            }
        }
        return valid
    }

    function createVpn(box){
        var connectionPath = dbusNetwork.CreateConnection(typeList.selectVpnType, "/")
        var sessionObject = connectionSession.createObject(rootPage, { path: connectionPath })
        for(var i=0; i<box.children.length; i++){
            var objLine = box.children[i]
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
            //sessionObject.Close()
        }
        stackView.reset()
    }

}
