import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"

Column {
    width: parent.width
    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Add Network Connection")
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: childrenRect.height
        color: DConstants.contentBgColor

        ListView {
            id: typeList
            width: parent.width
            height: childrenRect.height
            visible: count != 0

            property string selectItemId: "pppoe"
            model: ListModel {}

            Component.onCompleted: {
                model.append({
                    "item_id": "pppoe",
                    "item_name": dsTr("PPPoE")
                })
                /***
                model.append({
                    "item_id": "mobile",
                    "item_name": dsTr("Mobile 2G/3G/4G-LTE")
                })
                ***/
                model.append({
                    "item_id": "vpn",
                    "item_name": dsTr("VPN")
                })
            }

            delegate: SelectItem {
                totalItemNumber: typeList.count
                selectItemId: typeList.selectItemId

                onSelectAction: {
                    typeList.selectItemId = itemId
                }
            }
        } // End of typeList
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
            onClicked: {
                stackView.reset()
            }
        }

        DTextButton {
            text: dsTr("Next")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(typeList.selectItemId == "pppoe"){
                    gotoAddConnection("addDslPage", nmConnectionTypePppoe, "/")
                }
                else if(typeList.selectItemId == "mobile"){
                    gotoAddConnection("addMobilePage", nmConnectionTypeMobile, "/")
                }
                else if(typeList.selectItemId == "vpn"){
                    gotoAddConnection("addVpnPage", nmConnectionTypeVpn, "/")
                }
            }
        }
    }
}
