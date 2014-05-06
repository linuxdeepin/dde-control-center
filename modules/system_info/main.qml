import DBus.Com.Deepin.Daemon.SystemInfo 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    id: systemInfoModule
    anchors.fill: parent

    property var dconstants: DConstants {}
    property var systemInfoDbus: SystemInfo {}
    property var leftWidth: Math.max(versionItem.titleWidth, typeItem.titleWidth, cpuItem.titleWidth, memoryItem.titleWidth, distItem.titleWidth) + 8

    function bitToHuman(b, factor){
        if (b < factor) {
            return b + " B"
        }
        else{
            b = b/factor
            if ( b < factor ){
                return b.toFixed(2) +" KB"
            }
            else {
                b = b/factor
                if (b < factor){
                    return b.toFixed(2) + " MB"
                }
                else{
                    b = b/factor
                    if(b<10){
                        return b.toFixed(2) + " GB"
                    }
                    else if(b<100){
                        return b.toFixed(1) + " GB"
                    }
                    else{
                        return parseInt(b) + " GB"
                    }
                }
            }
        }
    }

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle { text: dsTr("System Information") }

        SepratorHorizontal {}

        Rectangle {
            id: logoBox
            width: parent.width
            height: 118
            color: dconstants.contentBgColor

            Image {
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "images/logo.png"
            }

            DssH3 {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter

                text: dsTr("Copyright (c) 2011-2014 Wuhan Deepin Technology Co.,Ltd.")
            }
        }

        InfoItem { 
            id: versionItem
            title: dsTr("Deepin Edition")
            value: systemInfoDbus.version
        }

        InfoItem { 
            id: typeItem
            title: dsTr("System Type")
            value: systemInfoDbus.systemType + dsTr("-bit")
        }

        InfoItem { 
            id: cpuItem
            title: dsTr("Processor")
            value: systemInfoDbus.processor
        }
        
        InfoItem { 
            id: memoryItem
            title: dsTr("Memory")
            value: bitToHuman(systemInfoDbus.memoryCap, 1024)
        }

        InfoItem { 
            id: distItem
            title: dsTr("Disk")
            value: bitToHuman(systemInfoDbus.diskCap, 1000)
        }

        Rectangle { 
            color: dconstants.contentBgColor
            height: 20
            width: parent.width
        }

        SepratorHorizontal {}
    }
}



