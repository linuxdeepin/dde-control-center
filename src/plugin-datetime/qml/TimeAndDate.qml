// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0
import org.deepin.dtk.private 1.0 as P

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
                property string longTimeFormat: dccData.longTimeFormat

                height: contentHeight
                Layout.leftMargin: 10
                font: DTK.fontManager.t1
                text: Qt.formatTime(Date(), longTimeFormat)
            }
            Label {
                id: dateLabel
                property bool needUpdate: false
                height: contentHeight
                Layout.leftMargin: 10
                font: DTK.fontManager.t5
                text: Qt.formatDate(Date(), dccData.longDateFormat)
            }

            Timer {
                interval: 500
                running: true
                repeat: true
                onTriggered: {
                    timeLabel.text = Qt.formatTime(Date(), timeLabel.longTimeFormat)

                    // TODO: 更新日期来说是否太频繁了？
                    dateLabel.text = Qt.formatDate(Date(), dccData.longDateFormat)
                }
            }
        }
    }

    // 时间同步
    DccObject {
        name: "dateTimeGroup"
        parentName: "dateTime"
        weight: 12
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => item ? item.topPadding = 10 : console.log("null parent..")

        DccObject {
            id: ntpSettings
            property bool ntpOn: dccData.ntpEnabled
            name: "ntpSettings"
            parentName: "dateTimeGroup"
            displayName: qsTr("auto sync time")
            weight: 10
            hasBackground: true
            pageType: DccObject.Editor
            page: Switch {
                checked: ntpSettings.ntpOn
                onCheckedChanged: {
                    dccData.ntpEnabled = checked
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
            page: Item {
                implicitHeight: 36
                implicitWidth: dccData.ntpEnabled ? 280 : 80

                ComboBox {
                    id: comboBox
                    flat: true
                    visible: dccData.ntpEnabled
                    anchors.fill: parent
                    model: dccData.ntpServerList
                    currentIndex: dccData.ntpServerList.indexOf(dccData.ntpServerAddress)
                    onActivated: function (index) {
                        dccData.ntpServerAddress = dccData.ntpServerList[index]
                    }
                }

                Button {
                    id: settingsButton
                    visible: !dccData.ntpEnabled
                    anchors.fill: parent
                    property bool needShowDialog: false
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
        }
        DccObject {
            name: "12/24h"
            parentName: "dateTimeGroup"
            displayName: qsTr("use 24-hour format")
            weight: 14
            hasBackground: true
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.use24HourFormat
                onCheckedChanged: {
                    dccData.use24HourFormat = checked
                }
            }
        }
    }

    // 系统时区
    DccObject {
        id: timezoneGroup
        name: "timezoneGroup"
        parentName: "dateTime"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {
            id: view
        }

        function addUserTimeZone(zoneId) {
            let displays = dccData.userTimeZoneText(0);
            let descritions = dccData.userTimeZoneText(1);
            let shifts = dccData.userTimeZoneText(2);
            let zoneIds = dccData.userTimeZoneText(3);
            for (let i = 0; i < displays.length && i < descritions.length; ++i) {
                if (zoneId && zoneId !== zoneIds[i])
                    continue

                var obj = itemZoneComp.createObject(timezoneGroup)
                obj.name = "timezoneItem" + i
                obj.parentName = "timezoneGroup"
                obj.displayName = displays[i]
                obj.description = descritions[i]
                obj.shift = shifts[i] / 3600
                obj.weight = 20 + 10 * (i + 1)
                DccApp.addObject(obj)
            }
        }

        Component.onCompleted: {
            timezoneGroup.addUserTimeZone(null)
        }
        Connections {
            target: dccData
            function onUserTimeZoneAdded(zoneInfo) {
                timezoneGroup.addUserTimeZone(zoneInfo.zoneName)
            }
        }

        onParentItemChanged: item => item ? item.topPadding = 10 : console.log("null parent..")
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
            implicitWidth: 280
            model: dccData.searchModel()
            textRole: "display"
            displayText: dccData.timeZoneDispalyName
            currentIndex: dccData.currentTimeZoneIndex

            popup: SearchableListViewPopup {
                id: searchView
                implicitWidth: combo.width
                delegateModel: combo.delegateModel
                maxVisibleItems: combo.maxVisibleItems
                Component.onCompleted: {
                    searchView.highlightedIndex = combo.currentIndex
                }

                onSearchTextChanged: {
                    let delegateModel = dccData.searchModel()
                    delegateModel.setFilterWildcard(searchView.searchText);
                }
            }

            onActivated: function (index) {
                let zoneId = currentValue["zoneIdRole"]
                dccData.systemTimeZone = zoneId
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
                        id: timezoneDialog
                        currentTimeZone: dccData.timeZoneDispalyName
                        currentLocation: dccData.country
                        onClosing: {
                            dccData.searchModel().setFilterRegularExpression("")
                            addButton.needShowDialog = false
                            if (timezoneDialog.saved) {
                                console.log("selectTimeZone", timezoneDialog.selectedTimeZone)
                                dccData.addUserTimeZoneByName(timezoneDialog.selectedTimeZone)
                            }
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
        }
    }

    Component {
        id: itemZoneComp
        DccObject {
            hasBackground: true
            pageType: DccObject.Item
            property int shift: 8
            page: ItemDelegate {
                id: itemDelegate
                visible: dccObj
                hoverEnabled: true
                implicitHeight: 50
                icon.name: dccObj.icon
                checkable: false
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ColumnLayout {
                        Layout.topMargin: 8
                        Layout.leftMargin: 60
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
                            font: DTK.fontManager.t10
                            text: dccObj.description
                            opacity: 0.5
                            elide: Text.ElideRight
                        }
                    }
                }

                TimezoneClock {
                    id: clock
                    width: 36
                    height: 36
                    shift: dccObj.shift
                    anchors {
                        left: itemDelegate.left
                        leftMargin: 10
                        top: itemDelegate.top
                        topMargin: (itemDelegate.height - clock.height) / 2
                    }
                }

                IconButton {
                    id: removeButton
                    visible: itemDelegate.hovered
                    icon.name: "user-trash-symbolic"
                    icon.width: 24
                    icon.height: 24
                    implicitWidth: 36
                    implicitHeight: 36
                    anchors {
                        right: itemDelegate.right
                        rightMargin: 10
                        // verticalCenter: itemDelegate // not work ?
                        top: itemDelegate.top
                        topMargin: (itemDelegate.height - removeButton.height) / 2

                    }
                    background: null
                    onClicked: {
                        console.log("need remove timezone", dccObj.displayName)
                        dccData.removeUserTimeZoneByName(dccObj.displayName)
                        DccApp.removeObject(dccObj)
                        dccObj.destroy(1000)
                    }
                }
                background: DccListViewBackground {
                    separatorVisible: true
                    highlightEnable: false
                }
            }
        }
    }

}
