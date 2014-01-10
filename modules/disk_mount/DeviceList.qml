import QtQuick 2.1
import Deepin.Widgets 1.0

ListView {
    height: count * 48
    model: ListModel {
        ListElement {
            deviceName: "Document"
            deviceType: "internal"
            deviceTotalStorage: 120
            deviceUsableStorage: 30
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
            deviceType: "removable"
            deviceTotalStorage: 120
            deviceUsableStorage: 50
            deviceMountable: true
            deviceMounted: false
        }
    }

    delegate: Item {
        id: deleaget
        width: ListView.view.width
        height: 48

        property string iconsDir: "/usr/share/icons/Deepin/devices/48/"

        Item{
            width: parent.width - 15 * 2
            height: parent.height - 9 * 2
            anchors.centerIn: parent

            Image {
                id: d_icon
                width: 36
                height: 36
                source: deviceType == "internal" ? iconsDir + "drive-harddisk.png" : iconsDir + "drive-removable-media-usb.png"

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            DssH3 {
                id: d_name
                text: deviceName

                anchors.top: parent.top
                anchors.left: d_storage_progress.left
            }

            DssH3 {
                id: d_storage_status
                text: deviceUsableStorage + "/" + deviceTotalStorage

                anchors.top: parent.top
                anchors.right: d_storage_progress.right
            }

            DImageButton {
                id: d_unmount_button

                normal_image: "images/eject_normal.png"
                hover_image: "images/eject_normal.png"
                press_image: "images/eject_press.png"

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }

            StorageBar {
                id: d_storage_progress
                percentage: deviceUsableStorage / deviceTotalStorage

                anchors.bottom: parent.bottom
                anchors.left: d_icon.right
                anchors.leftMargin: 10
                anchors.right: d_unmount_button.left
                anchors.rightMargin: 10
            }
        }
        DSeparatorHorizontal { anchors.bottom: parent.bottom }
    }
}
