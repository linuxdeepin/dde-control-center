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

import DBus.Com.Deepin.Daemon.SystemInfo 1.0
import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import Deepin.Widgets 1.0

Item {
    id: systemInfoModule
    anchors.fill: parent

    property var systemInfoDbus: SystemInfo {}
    property var leftWidth: Math.max(versionItem.titleWidth, typeItem.titleWidth, cpuItem.titleWidth, memoryItem.titleWidth, distItem.titleWidth) + 8

    property string lang: "en"

    Component.onCompleted: lang = dsslocale.lang

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


    DssTitle {
        id:sysinfoDT
        anchors.top: parent.top
        text: modulesId.moduleLocaleNames["system_info"]
    }

    SepratorHorizontal {anchors.top: sysinfoDT.bottom}

    ListView {
        id:sysinfoListView
        anchors.top:sysinfoDT.bottom
        height: parent.height - sysinfoDT.height
        width: parent.width
        model: itemModel
        clip: true
    }

    VisualItemModel
    {
        id: itemModel

        DSeparatorHorizontal{}

        Column {
            id: infoColumn
            width: sysinfoListView.width

            Rectangle {
                id: logoBox
                width: parent.width
                height: 118
                color: DPalette.contentBgColor

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

                    text: dsTr("Copyright (c) 2011-2014 Wuhan Deepin Technology Co., Ltd.")
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
                value: systemInfoDbus.systemType + dsTr("bit")
            }

            InfoItem {
                id: cpuItem
                title: dsTr("Processor")
                value: wrapCpuInfo(systemInfoDbus.processor)

                function wrapCpuInfo(s){
                    var index = s.indexOf("@")
                    if(index != -1){
                        s = s.replace("@", "\n@")
                    }
                    return s
                }
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
                color: DPalette.contentBgColor
                height: 20
                width: parent.width
            }

            SepratorHorizontal {}
        }

        DBaseExpand {
            id: gplExpand
            width: sysinfoListView.width
            height: systemInfoModule.height - infoColumn.height - sysinfoDT.height - 5
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("GNU GENERAL PUBLIC LICENSE")
                onClicked: {
                    gplExpand.expanded = !gplExpand.expanded
                }

                SepratorHorizontal {anchors.bottom: parent.bottom;visible: !gplExpand.expanded}
            }
            content.sourceComponent: DFlickable {
                width: parent.width
                height: gplExpand.height - 32
                contentWidth: parent.width
                contentHeight: gplContent.height

                Column {
                    id: gplContent
                    width: parent.width

                    TextEdit {
                        id: gplTextEditTitle
                        width: parent.width
                        color: DPalette.fgDarkColor
                        font.pixelSize: 11
                        text: mainObject.getGplText(lang, "title")
                        textMargin: 6
                        wrapMode: TextEdit.WordWrap
                        horizontalAlignment: TextEdit.AlignHCenter
                        readOnly: true
                    }

                    TextEdit {
                        id: gplTextEdit
                        width: parent.width
                        color: DPalette.fgDarkColor
                        font.pixelSize: 11
                        text: mainObject.getGplText(lang, "body")
                        textMargin: 6
                        wrapMode: TextEdit.WordWrap
                        readOnly: true
                    }
                }
            }

            SepratorHorizontal {
                anchors.bottom :parent.bottom
                width: sysinfoListView.width
            }
        }

    }



}



