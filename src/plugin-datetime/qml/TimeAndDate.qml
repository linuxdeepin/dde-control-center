// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

// 时间和日期
DccObject {
    DccObject {
        id: dateTimeContent
        name: "dateTimeContent"
        parentName: "dateTime"
        weight: 10
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                id: timeLabel
                height: contentHeight
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t1
                text: Qt.formatTime(Date(), "hh:mm:ss")
                Timer {
                    interval: 800
                    running: true
                    repeat: true
                    onTriggered: {
                        timeLabel.text = Qt.formatTime(Date(), "hh:mm:ss")
                    }
                }
            }
            Label {
                height: contentHeight
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t5
                // TODO change with  format settings
                text: Qt.formatDate(Date(), qsTr("yyyy年MM月dd日"))
            }
        }
    }

    // maybe have a pageType: DccObject.SpacingItem ?
    DccObject {
        name: "spacingItem0"
        parentName: "dateTime"
        weight: 11
        pageType: DccObject.Item
        page: Item {
            implicitHeight: 10
        }
    }

    // 时间同步
    DccObject {
        name: "dateTimeGroup"
        parentName: "dateTime"
        weight: 12
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            id: ntpSettings
            property bool ntpOn: false
            name: "ntpSettings"
            parentName: "dateTimeGroup"
            displayName: qsTr("auto sync time")
            weight: 10
            hasBackground: true
            pageType: DccObject.Editor
            page: D.Switch {
                checked: ntpSettings.ntpOn
                onCheckedChanged: {
                    ntpSettings.ntpOn = checked
                }
            }
        }
        DccObject {
            name: "dateAndTimeSettings"
            parentName: "dateTimeGroup"
            displayName: qsTr("system date and time")
            weight: 12
            hasBackground: true
            pageType: DccObject.Editor
            page: Button {
                // TODO: cmbobox and button
                id: settingsButton
                property bool needShowDialog: false
                implicitWidth: 80
                text: qsTr("Settings")

                Loader {
                    id: loader
                    active: settingsButton.needShowDialog
                    sourceComponent: DateTimeSettingDialog {
                        onClosing: {
                            settingsButton.needShowDialog = false
                        }
                    }
                    onLoaded: {
                        item.show()
                    }
                }

                onClicked: {
                    settingsButton.needShowDialog = true
                }
            }
        }
        DccObject {
            name: "12/24h"
            parentName: "dateTimeGroup"
            displayName: qsTr("use 24-hour format")
            weight: 14
            hasBackground: true
            pageType: DccObject.Editor
            page: D.Switch {
                checked: true
                onCheckedChanged: {

                }
            }
        }
    }

    // maybe have a pageType: DccObject.SpacingItem ?
    DccObject {
        name: "spacingItem"
        parentName: "dateTime"
        weight: 13
        pageType: DccObject.Item
        page: Item {
            implicitHeight: 10
        }
    }

    // 系统时区
    DccObject {
        name: "timezoneGroup"
        parentName: "dateTime"
        weight: 20
        // hasBackground: true
        pageType: DccObject.Item
        page: DccGroupView {
        }
    }

    DccObject {
        name: "systemTimezone"
        parentName: "timezoneGroup"
        displayName: qsTr("system time zone")
        weight: 12
        hasBackground: true
        pageType: DccObject.Editor
        page: ComboBox {
            id: combo
            flat: true
            implicitWidth: textMetrics.advanceWidth + 20
            model: ["乌鲁木齐（GMT+08：00）", "东京（GMT+09：00）", "北京（GMT+08：00）", "阿姆斯特丹（GMT-5）"]
            TextMetrics {
                id: textMetrics
                text: combo.currentText
            }
        }
    }
    DccObject {
        name: "timezoneList"
        parentName: "timezoneGroup"
        displayName: qsTr("Timezone list")
        weight: 12
        hasBackground: true
        pageType: DccObject.Editor
        page: RowLayout {
            spacing: 10
            Button {
                id: addButton
                property bool needShowDialog: false
                icon.name: "add"
                implicitHeight: 32
                implicitWidth: 32

                Loader {
                    id: timezoneLoader
                    active: addButton.needShowDialog
                    sourceComponent: TimezoneDialog {
                        onClosing: {
                            addButton.needShowDialog = false
                        }
                    }
                    onLoaded: {
                        item.show()
                    }
                }

                onClicked: {
                    addButton.needShowDialog = true
                }
            }
            Button {
                icon.name: "remove"
                implicitHeight: 32
                implicitWidth: 32
            }
        }
    }
    DccObject {
        name: "timezoneItem"
        parentName: "timezoneGroup"
        displayName: qsTr("乌鲁木齐（GMT+08：00）")
        description: qsTr("今天-8小时 上午12:08")
        icon: "dcc_nav_datetime"
        weight: 20
        hasBackground: true
        pageType: DccObject.Item
        page: ItemDelegate {
            implicitHeight: 50
            icon.name: dccObj.icon
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    Layout.topMargin: 8
                    Layout.leftMargin: 40
                    Layout.maximumWidth: 160

                    Label {
                        id: display
                        Layout.maximumWidth: 160
                        text: dccObj.displayName
                        elide: Text.ElideRight
                    }
                    Label {
                        id: description
                        Layout.maximumWidth: 160
                        visible: text !== ""
                        font: D.DTK.fontManager.t10
                        text: dccObj.description
                        opacity: 0.5
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
