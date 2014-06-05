/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: diskItem
    width: 224
    height: 68

    property var diskInfo: mountDiskList[index]
    property bool lastItem: index == mountDiskList.length - 1

    //Component.onCompleted: {
        //print(JSON.stringify(diskInfo))
    //}

    Connections {
        target: dbusDiskMount
        onError: {
            if(arg0 == diskInfo[7]){
                showError()
            }
        }
    }

    function showError(){
        unmountFailedInfo.visible = true
        sizeInfo.visible = false
        hideErrorTimer.restart()
    }

    Timer {
        id: hideErrorTimer
        interval: 1000
        onTriggered: {
            unmountFailedInfo.visible = false
            sizeInfo.visible = true
        }
    }

    Item {
        width: parent.width - xEdgePadding * 2
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter

        DssH2{
            id: title
            text: diskInfo[0]
        }

        Item {
            id: content
            height: diskItem.height - title.height - 5
            width: parent.width
            anchors.top: title.bottom
            anchors.topMargin: 2

            DIcon {
                id: diskIcon
                width: 48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
                theme: "Deepin"
                icon: externalObject.isPathExist(iconUrl) ? iconUrl : diskInfo[9]

                property url iconUrl: getIconUrl("disk_mount/%1.png".arg(diskInfo[9]))

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
                id: rightInfo
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - diskIcon.width - 10
                spacing: 6

                Item {
                    width: parent.width
                    height: childrenRect.height

                    DssH2 {
                        id: sizeInfo
                        text: "%1/%2".arg(bitToHuman(parseInt(diskInfo[4]))).arg(bitToHuman(parseInt(diskInfo[5])))
                        font.pixelSize: 11
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
            anchors.top: content.bottom
            anchors.topMargin: 2
            width: parent.width
            height: 1
            color: Qt.rgba(1, 1, 1, 0.1)
            visible: !lastItem
        }

    }
}
