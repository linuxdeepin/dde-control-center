import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Item {
    id: deviceItem
    width: parent.width
    height: 30

    property int bluetoothDeviceDisconnected: 0
    property int bluetoothDeviceConnecting: 1
    property int bluetoothDeviceConnected: 2

    // info from model
    property string itemId: typeof(item_id) == "undefined" ? "" : item_id
    property string itemName: typeof(item_name) == "undefined" ? "" : item_name
    property int itemState: typeof(item_state) == "undefined" ? "" : item_state
    // info from model

    signal connectDevice()
    signal disconnectDevice()

    DImageButton {
        id:checkImgButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
        normal_image: "qrc:/modules/bluetooth/images/connected.png"
        hover_image: "qrc:/modules/bluetooth/images/disconnect.png"
        visible: itemState == bluetoothDeviceConnected
        onClicked: {
            if (itemState == bluetoothDeviceDisconnected)
                deviceItem.connectDevice()
            else
                deviceItem.disconnectDevice()
        }
    }

    WaitingImage {
        anchors.left: checkImgButton.left
        anchors.verticalCenter: parent.verticalCenter
        on: itemState == bluetoothDeviceConnecting
    }

    DLabel {
        width: 220      // TODO need a preferred value
        anchors.left: parent.left
        anchors.leftMargin: 25
        anchors.verticalCenter: parent.verticalCenter
        elide: Text.ElideRight
        text: itemName
        font.pixelSize: 12

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = parent.DConstants.hoverColor
            onExited: parent.color = parent.DConstants.fgColor
            onClicked: {
                if (itemState == bluetoothDeviceDisconnected)
                    deviceItem.connectDevice()
                else
                    deviceItem.disconnectDevice()
            }
        }
    }
}
