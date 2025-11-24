// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0 as Dcc

D.DialogWindow {
    id: timezoneDialog
    property string currentTimeZone: "武汉 (UTC+08:00)"
    property string currentLocation: "武汉-中国大陆"
    property string selectedTimeZone
    property bool saved: false

    visible: true
    width: 1080
    height: 730
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    color: "#66000000"
    modality: Qt.WindowModal

    // remove titlebar background
    header: D.DialogTitleBar {
        enableInWindowBlendBlur: false
    }

    ColumnLayout {
        id: layout
        y: -20
        spacing: 10
        height: parent.height
        width: parent.width

        function showIndicator(zoneId, zoneDiplayName) {
            let size = zoneMap.sourceSize
            let zonePos = dccData.zonePosition(zoneId, size.width, size.height)

            let displaytexts = zoneDiplayName.split(' '); // [上海, (UTC+08:00)]

            indicator.x = zonePos.x - 6
            indicator.y = zonePos.y - 6
            zoneMap.timZonePos = Qt.point(indicator.x, indicator.y)
            timezoneDialog.selectedTimeZone = displaytexts[0]
            indicator.visible = true
        }

        Label {
            text: qsTr("Add time zone")
            font: D.DTK.fontManager.t2

            Layout.alignment: Qt.AlignCenter
        }

        Dcc.SearchBar {
            id: searchBar
            Layout.preferredWidth: 300
            Layout.alignment: Qt.AlignCenter
            model: dccData.zoneSearchModel()
            onClicked: function(model) {
                layout.showIndicator(model.zoneId, model.cityName)
            }
        }

        CheckBox {
            text: qsTr("Determine the time zone based on the current location")
            Layout.alignment: Qt.AlignCenter
            visible: false
        }

        D.DciIcon {
            id: zoneMap
            property point timZonePos

            name: "dcc_timezone_map"
            sourceSize: Qt.size(978, 500)
            Layout.alignment: Qt.AlignCenter

            ToolTip {
                id: toolTip
                property alias model: arrowListView.model
                contentItem: ColumnLayout {
                    Dcc.Repeater {
                        id: arrowListView
                        D.ItemDelegate {
                            id: item
                            implicitWidth: 120
                            implicitHeight: 30
                            text: modelData
                            background: Item {
                                implicitWidth: item.width
                                implicitHeight: item.height
                                Loader {
                                    anchors.fill: parent
                                    active: item.hovered
                                    sourceComponent: D.HighlightPanel {}
                                }
                            }
                            onClicked: {
                                timezoneDialog.selectedTimeZone = modelData
                                //toolTip.x = pos.x - 50 toolTip.y = pos.y - 10
                                zoneMap.timZonePos = Qt.point(toolTip.x + 50 - 6, toolTip.y + 10 - 6)
                                toolTip.hide()

                            }
                        }
                    }
                }

                onVisibleChanged: {
                    indicator.x = zoneMap.timZonePos.x
                    indicator.y = zoneMap.timZonePos.y
                    indicator.visible = !toolTip.visible && timezoneDialog.selectedTimeZone.length > 0
                }
            }

            D.DciIcon {
                id: indicator
                visible: !toolTip.visible && timezoneDialog.selectedTimeZone.length > 0
                name: "indicator_active"
                sourceSize:  Qt.size(12, 12)
            }

            ToolTip {
                id: holdToolTip
                x: indicator.x - holdToolTip.width / 2 + 6
                y: indicator.y - holdToolTip.height - 6
                closePolicy: Popup.NoAutoClose
                text: timezoneDialog.selectedTimeZone
                visible: indicator.visible && text.length > 0
            }

            function showTips(pos, model) {
                toolTip.visible = true
                toolTip.delay = 200
                toolTip.model = model
                toolTip.x = pos.x - 50
                toolTip.y = pos.y - 10
            }

            MouseArea {
                anchors.fill: parent
                onPressed: function(mouse) {
                    let zones = dccData.zones(mouse.x, mouse.y, zoneMap.width, zoneMap.height)
                    if (zones.length < 1) {
                        console.log("mouse pos:", mouse.x, mouse.y, "no zone found....");
                        return
                    }

                    indicator.visible = false

                    let zonePos = Qt.point(mouse.x, mouse.y)
                    zonePos = dccData.zonePosition(zones[0], zoneMap.width, zoneMap.height)
                    Qt.callLater(zoneMap.showTips, zonePos, zones);
                }
            }

            Component.onCompleted: {
                // show with indicator
                layout.showIndicator(dccData.systemTimeZone, dccData.timeZoneDispalyName)
            }
        }

        RowLayout {
            visible: false
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
            visible: false
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
                onClicked: {
                    timezoneDialog.saved = false
                    close()
                }
            }
            D.RecommandButton {
                enabled: timezoneDialog.selectedTimeZone.length > 0
                Layout.bottomMargin: 10
                text: qsTr("Save")
                onClicked: {
                    timezoneDialog.saved = true
                    close()
                }
            }
        }
    }
}
    
