import QtQuick 2.1
import DBus.Com.Deepin.Daemon.SystemInfo 1.0

Rectangle {
    id: systemInfoModule
    anchors.fill: parent
    color: childColor

    property color textColor: "#b4b4b4"

    property var systemInfoDbus: SystemInfo {}

    Text {
        id: systemInfoTitle
        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: 15
        font.bold: true
        color: textColor
        text: dsTr("System Information")
    }

    SepratorHorizontal {
        anchors.top: parent.top
        anchors.topMargin: 46
    }

    Rectangle {
        id: logoBox
        anchors.top: parent.top
        anchors.topMargin: 48
        width: parent.width
        height: 118
        color: "#1a1b1b"

        Image {
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            source: "images/logo.png"
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 18
            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: 10
            color: textColor
            text: dsTr("Copyright (c) 2011-2014 武汉深之度科技有限公司 版权所有")
        }
    }

    SepratorHorizontal { anchors.top: logoBox.bottom }

    InfoItem { 
        id: versionItem
        anchors.top: logoBox.bottom
        anchors.topMargin: 2
        title: dsTr("System Version")
        value: dsTr("2014")
    }

    SepratorHorizontal { anchors.top: versionItem.bottom }

    InfoItem { 
        id: typeItem
        anchors.top: versionItem.bottom
        anchors.topMargin: 2
        title: dsTr("System Type")
        value: dsTr("32Bit")
    }

    SepratorHorizontal { anchors.top: typeItem.bottom }

    InfoItem { 
        id: cpuItem
        anchors.top: typeItem.bottom
        anchors.topMargin: 2
        title: dsTr("CPU")
        value: dsTr("Intel Core")
    }
    SepratorHorizontal { anchors.top: cpuItem.bottom }
    
    InfoItem { 
        id: memoryItem
        anchors.top: cpuItem.bottom
        anchors.topMargin: 2
        title: dsTr("Memory")
        value: dsTr("4GB")
    }
    SepratorHorizontal { anchors.top: memoryItem.bottom }

    InfoItem { 
        id: distItem
        anchors.top: memoryItem.bottom
        anchors.topMargin: 2
        title: dsTr("Disk")
        value: dsTr("500GB")
    }
    SepratorHorizontal { anchors.top: distItem.bottom }

}
