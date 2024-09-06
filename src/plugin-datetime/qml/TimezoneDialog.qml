// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
        
D.DialogWindow {
    id: timezoneDialog
    // TODO: get current timezone from backend
    property string currentTimeZone: "中国 (GMT+8)"
    property string currentLocation: "武汉-中国大陆"

    visible: true
    width: 1080
    height: 830
    color: "#66000000"
    modality: Qt.WindowModal

    // remove titlebar background
    header: D.DialogTitleBar {
        enableInWindowBlendBlur: true
        background: null
    }

    ColumnLayout {
        y: -20
        spacing: 10
        height: parent.height
        width: parent.width

        Label {
            text: qsTr("Add time zone")
            font: D.DTK.fontManager.t2

            Layout.alignment: Qt.AlignCenter
        }

        D.SearchEdit {
            Layout.preferredWidth: 300
            placeholder: qsTr("Search")
            Layout.alignment: Qt.AlignCenter
        }

        CheckBox {
            text: qsTr("Determine the time zone based on the current location")
            Layout.alignment: Qt.AlignCenter
        }

        D.DciIcon {
            id: zoneMap
            name: "dcc_timezone_map"
            sourceSize: Qt.size(978, 500)
            Layout.alignment: Qt.AlignCenter

            ToolTip {
                id: toolTip
            }

            function showTips(text, pos) {
                toolTip.text = text
                toolTip.visible = true
                toolTip.delay = 500
                toolTip.x = pos.x
                toolTip.y = pos.y - 20
            }

            MouseArea {
                anchors.fill: parent
                onPressed: function(mouse) {
                    let zones = dccData.zones(mouse.x, mouse.y, zoneMap.width, zoneMap.height)
                    if (zones.length < 1) {
                        console.log("mouse pos:", mouse.x, mouse.y, "no zone found....");
                        return
                    }

                    let text = zones[0]
                    if (zones.length > 1)
                        text = zones.join("\n")

                    let zonePos = Qt.point(mouse.x, mouse.y)
                    zonePos = dccData.zonePosition(zones[0], zoneMap.width, zoneMap.height)
                    console.log("mouse pos:", mouse.x, mouse.y, "zone pos:", zonePos.x, zonePos.y);

                    Qt.callLater(zoneMap.showTips, text, zonePos);
                }
            }
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: 10
            Label {
                text: qsTr("Time zone:")
            }
            Label {
                text: timezoneDialog.currentTimeZone
            }
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: 30
            Label {
                text: qsTr("Nearest City:")
            }
            Label {
                text: timezoneDialog.currentLocation
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignCenter | Qt.AlignBottom
            spacing: 10
            Button {
                Layout.bottomMargin: 10
                text: qsTr("Cancel")
            }
            D.RecommandButton {
                Layout.bottomMargin: 10
                text: qsTr("Save")
            }
        }
    }
}
    
