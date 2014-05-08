import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"

Column {
    width: parent.width
    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("新建VPN连接")
        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        leftLoader.sourceComponent: DssH3{
            text: dsTr("选择类型")
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

            property string selectItemId: "pptp"
            model: ListModel {}

            Component.onCompleted: {
                model.append({
                    "item_id": "pptp",
                    "item_name": dsTr("PPTP")
                })
                model.append({
                    "item_id": "openvpn",
                    "item_name": dsTr("OPENVPN")
                })
                model.append({
                    "item_id": "l2tp",
                    "item_name": dsTr("L2TP")
                })
                model.append({
                    "item_id": "ipsec",
                    "item_name": dsTr("IPSEC")
                })
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

        property int edgePadding: 24
        property int leftWidth: edgePadding
        property int rightWidth: addVpnBox.width - leftWidth - edgePadding

        function updateLeftWidth(newWidth){
            if(newWidth + edgePadding > addVpnBox.leftWidth){
                addVpnBox.leftWidth = newWidth + edgePadding
            }
        }

        DCenterLine {
            id: vpnName
            title.text: dsTr("Name")
            leftWidth: addVpnBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
            }
            Component.onCompleted: {
                addVpnBox.updateLeftWidth(title.contentWidth)
            }
        }

        DCenterLine {
            id: vpnServer
            title.text: dsTr("Server")
            leftWidth: addVpnBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
            }
            Component.onCompleted: {
                addVpnBox.updateLeftWidth(title.contentWidth)
            }
        }

        DCenterLine {
            id: vpnUserName
            title.text: dsTr("Account")
            leftWidth: addVpnBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
            }
            Component.onCompleted: {
                addVpnBox.updateLeftWidth(title.contentWidth)
            }
        }

        DCenterLine {
            id: vpnPassword
            title.text: dsTr("Password")
            leftWidth: addVpnBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addVpnBox.rightWidth
            }
            Component.onCompleted: {
                addVpnBox.updateLeftWidth(title.contentWidth)
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
            text: dsTr("Finish")
            anchors.verticalCenter: parent.verticalCenter
        }
    }

}
