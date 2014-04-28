import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: diskItem
    width: 240
    height: lastItem ? childrenRect.height : childrenRect.height + 6

    property var diskInfo: mountDiskList[index]
    property bool lastItem: index == mountDiskList.length - 1

    Column {
        width: parent.width
        spacing: 4

        DssH2{
            text: diskInfo[1]
        }

        Row {
            height: 50
            spacing: 10

            Image {
                id: diskIcon
                source: "images/usb_disk.png" 
                anchors.verticalCenter: parent.verticalCenter
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                width: diskItem.width - diskIcon.width - 10
                spacing: 6

                Item {
                    width: parent.width
                    height: childrenRect.height

                    DssH2 {
                        text: "%1/%2".arg(bitToHuman(parseInt(diskInfo[5]))).arg(bitToHuman(parseInt(diskInfo[6])))
                    }

                    DssH2 {
                        id: unmountFailedInfo
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        font.pixelSize: 11
                        color: dconstants.tuhaoColor
                        text: dsTr("Unmount failed!")
                        visible: false
                    }

                    DOpacityImageButton {
                        anchors.right: parent.right
                        width: 16
                        height: 16
                        source: "images/unmount_button.png"

                        onClicked: {
                            var r = dbusDiskMount.DeviceUnmount(diskInfo[0])
                            print("==> [info] Unmount status:", r)
                            unmountFailedInfo.visible = !r[0]
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 4
                    color: Qt.rgba(1, 1, 1, 0.1)

                    Rectangle {
                        width: parent.width * diskInfo[5]/diskInfo[6]
                        height: parent.height
                        color: Qt.rgba(1, 1, 1, 0.6)
                    }
                }
            }

        }

        Rectangle {
            width: parent.width * 0.96
            height: 1
            color: Qt.rgba(1, 1, 1, 0.1)
            visible: !lastItem
            anchors.horizontalCenter: parent.horizontalCenter
        }

    }
}
