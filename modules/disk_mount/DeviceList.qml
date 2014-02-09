import QtQuick 2.1
import Deepin.Widgets 1.0

ListView {
    height: count * 48
    model: ListModel {}

    delegate: Item {
        id: deleaget
        width: ListView.view.width
        height: 48

        property string iconsDir: "/usr/share/icons/Deepin/devices/48/"

        function getHumanReadable(capacity) {
            var teras = capacity / (1024 * 1024 * 1024)
            capacity = capacity % (1024 * 1024 * 1024)
            var gigas = capacity / (1024 * 1024)
            capacity = capacity % (1024 * 1024)
            var megas = capacity / 1024
            capacity = capacity % 1024

            return Math.floor(teras) ? teras.toFixed(1) + "TB" :
            Math.floor(gigas) ? gigas.toFixed(1) + "GB":
            Math.floor(megas) ? megas.toFixed(1) + "MB" :
            capacity + "KB"
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                if (!deviceCanUnmount) {
                    dbus_mounts.DeviceMount(deviceId)
                }
            }
        }

        Item{
            width: parent.width - 15 * 2
            height: parent.height - 9 * 2
            anchors.centerIn: parent

            Image {
                id: d_icon
                width: 36
                height: 36
                source: deviceType == "native" ? iconsDir + "drive-harddisk.png" : iconsDir + "drive-removable-media-usb.png"

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
                visible: deviceCanUnmount && deviceType != "network"
                text: getHumanReadable(deviceTotalCap - deviceUsableCap) + "/" + getHumanReadable(deviceTotalCap)

                anchors.top: parent.top
                anchors.right: d_storage_progress.right
            }

            DImageButton {
                id: d_unmount_button
                visible: deviceCanUnmount

                normal_image: "images/eject_normal.png"
                hover_image: "images/eject_normal.png"
                press_image: "images/eject_press.png"

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    dbus_mounts.DeviceUnmount(deviceId)
                }
            }

            StorageBar {
                id: d_storage_progress
                visible: d_storage_status.visible
                percentage: (deviceTotalCap - deviceUsableCap) / (deviceTotalCap + 1)

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
