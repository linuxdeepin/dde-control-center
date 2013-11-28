import QtQuick 2.1
import DBus.Com.Deepin.Daemon.SystemInfo 1.0

Rectangle {
    id: systemInfoModule
    anchors.fill: parent
    color: childColor

    property color textColor: "#b4b4b4"

    property var systemInfoDbus: SystemInfo {}

    function bitToHuman(b){
        if (b < 1024) {
            return b + " B"
        }
        else{
            b = b/1024
            if ( b < 1024 ){
                return b.toFixed(2) +" KB"
            }
            else {
                b = b/1024
                if (b < 1024){
                    return b.toFixed(2) + " MB"
                }
                else{
                    b = b/1024
                    return b.toFixed(2) + " GB"
                }
            }
        }
    }

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
            //text: dsTr("Copyright (c) 2011-2014 武汉深之度科技有限公司 版权所有")
            text: dsTr("Copyright (c) 2011-2014 Wuhan Deepin Technology Co.,Ltd.")
        }
    }

    InfoItem { 
        id: versionItem
        anchors.top: logoBox.bottom
        title: dsTr("System Version")
        value: systemInfoDbus.version
    }

    InfoItem { 
        id: typeItem
        anchors.top: versionItem.bottom
        title: dsTr("System Type")
        value: systemInfoDbus.systemType + dsTr("Bit")
    }


    InfoItem { 
        id: cpuItem
        anchors.top: typeItem.bottom
        title: dsTr("CPU")
        value: systemInfoDbus.processor
    }
    
    InfoItem { 
        id: memoryItem
        anchors.top: cpuItem.bottom
        title: dsTr("Memory")
        value: bitToHuman(systemInfoDbus.memoryCap)
    }

    InfoItem { 
        id: distItem
        anchors.top: memoryItem.bottom
        title: dsTr("Disk")
        value: bitToHuman(systemInfoDbus.diskCap)
    }

    Rectangle { 
        color: "#1a1b1b"
        height: 15
        width: parent.width
        anchors.top: distItem.bottom 
    }

    SepratorHorizontal { 
        anchors.top: distItem.bottom 
        anchors.topMargin: 15
    }

}
