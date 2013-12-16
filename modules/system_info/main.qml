import DBus.Com.Deepin.Daemon.SystemInfo 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets"

Item {
    anchors.fill: parent

    property var dconstants: DConstants {}
    property var systemInfoDbus: SystemInfo {}

    function bitToHuman(b){
        if (b < 1000) {
            return b + " B"
        }
        else{
            b = b/1000
            if ( b < 1000 ){
                return b.toFixed(2) +" KB"
            }
            else {
                b = b/1000
                if (b < 1000){
                    return b.toFixed(2) + " MB"
                }
                else{
                    b = b/1000
                    return b.toFixed(2) + " GB"
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

            DssH4 {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter

                text: dsTr("Copyright (c) 2011-2014 Wuhan Deepin Technology Co.,Ltd.")
            }
        }

        InfoItem { 
            id: versionItem
            title: dsTr("System Version")
            value: systemInfoDbus.version
        }

        InfoItem { 
            id: typeItem
            title: dsTr("System Type")
            value: systemInfoDbus.systemType + dsTr("Bit")
        }

        InfoItem { 
            id: cpuItem
            title: dsTr("CPU")
            value: systemInfoDbus.processor
        }
        
        InfoItem { 
            id: memoryItem
            title: dsTr("Memory")
            value: bitToHuman(systemInfoDbus.memoryCap)
        }

        InfoItem { 
            id: distItem
            title: dsTr("Disk")
            value: bitToHuman(systemInfoDbus.diskCap)
        }

        ////Rectangle { 
            ////color: dconstants.contentBgColor
            ////height: 15
            ////width: parent.width
            ////anchors.top: distItem.bottom 
        ////}

        SepratorHorizontal {}
    }
}



