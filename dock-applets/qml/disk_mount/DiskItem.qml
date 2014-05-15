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
            text: diskInfo[0]
        }

        Row {
            height: 50
            spacing: 10

            Image {
                id: diskIcon
                source: "images/usb_disk.png" 
                anchors.verticalCenter: parent.verticalCenter

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        externalObject.xdgOpen(diskInfo[8])
                    }
                    onEntered: cursorShape = Qt.PointingHandCursor
                    onExited: cursorShape = Qt.ArrowCursor
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                width: diskItem.width - diskIcon.width - 10
                spacing: 6

                Item {
                    width: parent.width
                    height: childrenRect.height

                    DssH2 {
                        text: "%1/%2".arg(bitToHuman(parseInt(diskInfo[4]))).arg(bitToHuman(parseInt(diskInfo[5])))
                    }

                    DssH2 {
                        id: unmountFailedInfo
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        font.pixelSize: 11
                        color: dconstants.tuhaoColor
                        text: dsTr("Unmount failed!")
                        visible: false

                        Connections {
                            target: dbusDiskMount
                            onError: {
                                if(arg0 == diskInfo[7]){
                                    unmountFailedInfo.visible = true
                                }
                            }
                        }
                    }

                    DOpacityImageButton {
                        anchors.right: parent.right
                        width: 16
                        height: 16
                        source: "images/unmount_button.png"

                        onClicked: {
                            var r = dbusDiskMount.DeviceUnmount(diskInfo[7])
                            print("==> [info] Unmount status:", r)
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 4
                    color: Qt.rgba(1, 1, 1, 0.1)

                    Rectangle {
                        width: parent.width * diskInfo[4]/diskInfo[5]
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
