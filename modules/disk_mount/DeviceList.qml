import QtQuick 2.1

ListView {
    model: ListModel {
        ListElement {
            deviceName: "Document"
            deviceType: "internal"
            deviceTotalStorage: 120
            deviceUsableStorage: 70
            deviceMountable: true
            deviceMounted: false
        }
        ListElement {
            deviceName: "Document"
            deviceType: "internal"
            deviceTotalStorage: 120
            deviceUsableStorage: 70
            deviceMountable: true
            deviceMounted: false
        }
        ListElement {
            deviceName: "Document"
            deviceType: "internal"
            deviceTotalStorage: 120
            deviceUsableStorage: 70
            deviceMountable: true
            deviceMounted: false
        }
    }

    delegate: Item {
        id: deleaget
        width: ListView.view.width
        height: 36

        property string iconsDir: "/usr/share/icons/Deepin/devices/48/"

        Item {
            width: parent.width - 15 * 2
            height: parent.height - 10 * 2
            anchors.centerIn: parent

            Image {
                id: d_icon
                width: 36
                height: 36
                source: deviceType == "internal" ? iconsDir + "drive-harddisk.png" : iconsDir + "drive-removable-media-usb.png"

                anchors.left: parent.left
            }

            DssH3 {
                id: d_name
                text: deviceName

                anchors.top: parent.top
                anchors.left: d_storage_progress.right
            }

            DssH3 {
                id: d_storage_status
                text: deviceUsableStorage + "/" deviceTotalStorage

                anchors.top: parent.top
                anchors.left: d_storage_progress.right
            }

            StorageBar {
                anchors.bottom: parent.bottom
                anchors.left: d_icon.right
                anchors.leftMargin: 10
                anchors.right: parent.right
            }
        }
    }
}





