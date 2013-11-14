import QtQuick 2.0

Rectangle {
    id:system_info
    width: logo.implicitWidth
    height: 768

    property int fontSize: 15
	property int infoLeftMargin: 40
	property int infoBottomMargin: 20

    Image {
        id: logo
        source: "images/logo.png"
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
    }

    Rectangle {
        id: box
        width: parent.width
		height: row.height

        anchors {
            top: logo.bottom
			left: parent.left
			leftMargin: system_info.infoLeftMargin
         }

        Row {
			id: row
			height: keyVersion.implicitHeight + keyCpu.implicitHeight + keyMem.implicitHeight
			+ keyArch.implicitHeight + keyDisk.implicitHeight
			spacing: 15
            Column {
                id: key
                Text {
                    id: keyVersion
                    text: qsTr("Version")
                }
                Text {
                    id: keyCpu
                    text: qsTr("CPU")
                }
                Text {
                    id: keyMem
                    text: qsTr("Memory")
                }
                Text {
                    id: keyArch
                    text: qsTr("OS Type")
                }
                Text {
                    id: keyDisk
                    text: qsTr("Disk")
                }
            }

            Column {
                id: value
                Text {
                    id: valueVersion
                    text: module_system_info.getVersionInfo()
                }
                Text {
                    id: valueCpu
                    text: module_system_info.getCpuInfo()
                }
                Text {
                    id: valueMem
                    text: module_system_info.getMemoryInfo()
                }
                Text {
                    id: valueArch
                    text: module_system_info.getArchitectureInfo()
                }
                Text {
                    id: valueDisk
                    text: module_system_info.getDiskInfo()
                }
            }
        }

        Text {
            id: company_info
            text: qsTr("Copyright © 2011 - 2013 \nWuhan Deepin Technology Co.Ltd \nAll rights reserved")
			anchors {
				top: box.bottom
				topMargin: system_info.infoBottomMargin 
			}
        }
    }
}