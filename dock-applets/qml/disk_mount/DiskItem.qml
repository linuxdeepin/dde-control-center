
import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    width: 240
    height: lastItem ? childrenRect.height : childrenRect.height + 6

    property var diskInfo: mountDiskList[index]
    property bool lastItem: index == mountDiskList.length - 1

    Column {
        width: parent.width
        spacing: 4

        DssH2{
            text: diskInfo[1]
            anchors.left: parent.left
            anchors.leftMargin: 4
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
                width: 175
                spacing: 6

                Item {
                    width: parent.width
                    height: childrenRect.height

                    DssH2 {
                        text: "%1/%2".arg(bitToHuman(parseInt(diskInfo[5]))).arg(bitToHuman(parseInt(diskInfo[6])))
                    }

                    DOpacityImageButton {
                        anchors.right: parent.right
                        width: 16
                        height: 16
                        source: "images/unmount_button.png"

                        onClicked: dbusDiskMount.DeviceUnmount(diskInfo[0])
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
