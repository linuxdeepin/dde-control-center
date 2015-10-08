import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0
import "../shared/"

Column {
    id:rootExpand
    width: parent.width

    property int adapterCount: 0
    property int adapterIndex: 0
    property var bluetoothAdapter
    property int deviceDisconnected: 0
    property int deviceConnecting: 1
    property int deviceConnected: 2
    property string adapterName: bluetoothAdapter.Alias
    property int maxNameWidth: 200

    function isNameInUsed(name){
        for (var i = 0; i < adapterCount; i ++){
            if (adapters[i].Alias == name)
                return true
        }
        return false
    }

    Item {
        width: parent.width
        height: childrenRect.height
        DBaseLine {
            anchors.top: parent.top
            height: 38
            visible: editItem.height == 0
            leftLoader.sourceComponent: Item {
                DssH2 {
                    id:adapterNameText
                    anchors.left: parent.left
                    elide: Text.ElideRight
                    anchors.verticalCenter: parent.verticalCenter
                    text: bluetoothAdapter.Alias

                    function updateWidth(){
                        var tmpWidth = getStringPixelSize(adapterName,13)
                        if (tmpWidth < maxNameWidth)
                            adapterNameText.width = tmpWidth
                        else
                            adapterNameText.width = maxNameWidth
                    }

                    Connections{
                        target: rootExpand
                        onAdapterNameChanged: adapterNameText.updateWidth()
                    }

                    Component.onCompleted: adapterNameText.updateWidth()
                }

                DImageButton {
                    id:editImg
                    width: 14
                    height: 14
                    anchors.left: adapterNameText.right
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    normal_image: "qrc:/modules/bluetooth/images/edit_normal.png"
                    hover_image: "qrc:/modules/bluetooth/images/edit_hover.png"
                    onClicked: {
                        //TUDO
                        editItem.height = 60
                    }
                }
            }

            rightLoader.sourceComponent: DSwitchButton {
                property var pAdapterPower: bluetoothAdapter.Powered
                onPAdapterPowerChanged: {
                    print ("==>[info] Adapter power changed:",pAdapterPower)
                    checked = pAdapterPower
                }

                onClicked: dbus_bluetooth.SetAdapterPowered(bluetoothAdapter.Path, checked)
            }
        }

        Item {
            id:editItem
            width: parent.width
            height: 0
            visible: height != 0
            anchors.top: parent.top
            anchors.topMargin: 6

            Behavior on height {
                PropertyAnimation {target: editItem;property:"height"; easing.type: Easing.InQuart; duration: 300}
            }

            DTextInput{
                id:nameInput
                text:bluetoothAdapter.Alias
                width: parent.width - 20
                height: parent.height * 2 / 5
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
            }

            DTextButton {
                id:cancelButton
                text: dsTr("Cancel")
                anchors.top: applyButton.top
                anchors.right: applyButton.left
                anchors.rightMargin: 3
                onClicked: {
                    editItem.height = 0
                }

            }

            DTextButton {
                id:applyButton
                text: dsTr("Apply")
                anchors.top: nameInput.bottom
                anchors.topMargin: 8
                anchors.right: nameInput.right
                onClicked: {
                    editItem.height = 0

                    if (isNameInUsed(nameInput.text.trim()) && nameInput.text.trim() == "")
                        return
                    else
                        dbus_bluetooth.SetAdapterAlias(bluetoothAdapter.Path, nameInput.text.trim())
                }
            }

        }
    }

    DSeparatorHorizontal {}

    DBaseExpand {
        id: bluetoothDeviceExpand
        width: parent.width
        height: childrenRect.height
        visible: bluetoothAdapter.Powered
        expanded: bluetoothAdapter.Powered

        header.sourceComponent: DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Devices nearby")
            }
            rightLoader.sourceComponent: RefreshButton {
                // TODO
                rotating: bluetoothAdapter.Discovering
                onClicked: {
                    if (rotating)
                        return
                    print ("==> Discovering devices...")
                    dbus_bluetooth.RequestDiscovery(bluetoothAdapter.Path)
                }
            }
        }

        content.sourceComponent: ListView{
            width: parent.width
            height: childrenRect.height

            model: ListModel {
                id: deviceModel
                Component.onCompleted: {
                    var devInfos = unmarshalJSON(dbus_bluetooth.GetDevices(bluetoothAdapter.Path))
                    clear()
                    for(var i in devInfos){
                        addOrUpdateDevice(devInfos[i])
                    }
                }
                function addOrUpdateDevice(devInfo) {
                    if (isDeviceExists(devInfo)) {
                        updateDevice(devInfo)
                    } else {
                        addDevice(devInfo)
                    }
                }
                function addDevice(devInfo) {
                    var insertIndex = getInsertIndex(devInfo)
                    print("-> addBluetoothDevice", insertIndex)
                    insert(insertIndex, {
                        "devInfo": devInfo,
                        "adapter_path": devInfo.AdapterPath,
                        "item_id": devInfo.Path,
                        "item_name": devInfo.Alias,
                        "item_state":devInfo.State
                    })
                }
                function updateDevice(devInfo) {
                    var i = getDeviceIndex(devInfo)
                    get(i).devInfo = devInfo
                    get(i).item_name = devInfo.Alias
                    get(i).item_state = devInfo.State
                    sortModel()
                }
                function removeDevice(devInfo) {
                    if (isDeviceExists(devInfo)) {
                        var i = getDeviceIndex(devInfo)
                        remove(i, 1)
                    }
                }
                function isDeviceExists(devInfo) {
                    if (getDeviceIndex(devInfo) != -1) {
                        return true
                    }
                    return false
                }
                function getDeviceIndex(devInfo) {
                    for (var i=0; i<count; i++) {
                        if (get(i).devInfo.Path == devInfo.Path) {
                            return i
                        }
                    }
                    return -1
                }
                function getInsertIndex(devInfo) {
                    for (var i=0; i<count; i++) {
                        if (devInfo.RSSI >= get(i).devInfo.RSSI) {
                            return i
                        }
                    }
                    return count
                }
                function sortModel() {
                    var n;
                    var i;
                    for (n=0; n<count; n++) {
                        for (i=n+1; i<count; i++) {
                            if (get(n).devInfo.RSSI+5 < get(i).devInfo.RSSI) {
                                move(i, n, 1);
                                n=0; // Repeat at start since I can't swap items i and n
                            }
                        }
                    }
                }
            }

            delegate: BluetoothDeviceItem{
                onConnectDevice: dbus_bluetooth.ConnectDevice(itemId)
                onDisconnectDevice: dbus_bluetooth.DisconnectDevice(itemId)
            }

            Connections {
                target: dbus_bluetooth
                onDeviceAdded: {
                    var devInfo = unmarshalJSON(arg0)
                    if (devInfo.AdapterPath != bluetoothAdapter.Path)
                        return
                    deviceModel.addOrUpdateDevice(devInfo)
                }
                onDeviceRemoved: {
                    var devInfo = unmarshalJSON(arg0)
                    if (devInfo.AdapterPath != bluetoothAdapter.Path)
                        return
                    deviceModel.removeDevice(devInfo)
                }
                onDevicePropertiesChanged: {
                    var devInfo = unmarshalJSON(arg0)
                    if (devInfo.AdapterPath != bluetoothAdapter.Path)
                        return
                    deviceModel.addOrUpdateDevice(devInfo)
                }
            }
        }

    }

    DSeparatorHorizontal {visible: bluetoothAdapter.Powered}

}
