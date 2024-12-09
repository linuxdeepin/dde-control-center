// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQml.Models 2.11
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0
import org.deepin.dtk.private 1.0 as P

// 时间和日期
DccObject {
    DccObject {
        id: dateTimeContent
        name: "dateTimeContent"
        parentName: "timeAndLang"
        weight: 10
        pageType: DccObject.Item
        page: ColumnLayout {

            FontLoader {
                id: webFont
                source: "qrc:/builtin-font/resource/Outfit-Light.ttf"
            }

            Label {
                id: timeLabel
                height: contentHeight
                Layout.leftMargin: 10
                font {
                    pointSize: 40
                    family: webFont.font.family
                 }
                text: dccData.currentTime
            }
            Label {
                id: dateLabel
                height: contentHeight
                Layout.leftMargin: 10
                font {
                    pointSize: 14
                    family: webFont.font.family
                }
                text: dccData.currentDate
            }
        }
    }

    // 时间同步
    DccObject {
        name: "dateTimeGroup"
        parentName: "timeAndLang"
        weight: 12
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => { if (item) item.topPadding = 10 }

        DccObject {
            id: ntpSettings
            property bool ntpOn: dccData.ntpEnabled
            name: "ntpSettings"
            parentName: "dateTimeGroup"
            displayName: qsTr("Auto sync time")
            weight: 10
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Switch {
                checked: ntpSettings.ntpOn
                onCheckedChanged: {
                    dccData.ntpEnabled = checked
                }
            }
        }

        DccObject {
            id: dateAndTimeSettings
            name: "dateAndTimeSettings"
            parentName: "dateTimeGroup"
            displayName: dccData.ntpEnabled ? qsTr("Ntp server") : qsTr("System date and time")
            weight: 12
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            property bool showCustom: false
            property string customAddr
            page: Item {
                implicitHeight: 36
                implicitWidth: dccData.ntpEnabled ? 280 : 80

                ComboBox {
                    id: comboBox
                    property var serverList: dccData.ntpServerList
                    flat: true
                    visible: dccData.ntpEnabled
                    anchors.fill: parent
                    hoverEnabled: true
                    model: serverList
                    // 不设置默认的话可能无法滚动（不显示上下箭头按钮）。。。
                    maxVisibleItems: serverList.length - 1
                    currentIndex:  {
                        let index = serverList.indexOf(dccData.ntpServerAddress)
                        dateAndTimeSettings.showCustom = (index < 0)
                        if (index < 0)
                            dateAndTimeSettings.customAddr = dccData.ntpServerAddress
                        return index < 0 ? serverList.length - 1 : index
                    }
                    onActivated: function (index) {
                        dateAndTimeSettings.showCustom = (serverList[index] === qsTr("Customize"))
                        if (dateAndTimeSettings.showCustom) {
                            if (dateAndTimeSettings.customAddr.length > 0)
                                dccData.ntpServerAddress = dateAndTimeSettings.customAddr
                            return
                        }

                        dccData.ntpServerAddress = serverList[index]
                    }

                    Component.onCompleted: {
                        let text = qsTr("Customize")
                        if (!comboBox.serverList.includes(text))
                            comboBox.serverList.push(text)

                        if (comboBox.currentIndex < 0) {
                            comboBox.currentIndex = comboBox.serverList.length - 1
                            dateAndTimeSettings.showCustom = true
                            dateAndTimeSettings.customAddr = dccData.ntpServerAddress
                        }
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
            onDeactive: {
                if (dateAndTimeSettings.showCustom &&
                        dateAndTimeSettings.customAddr.length === 0) {
                    dateAndTimeSettings.showCustom = false
                }
            }
        }
        DccObject {
            id: customNTPServer
            name: "customNTPServer"
            parentName: "dateTimeGroup"
            displayName: qsTr("Server address")
            visible: dccData.ntpEnabled && dateAndTimeSettings.showCustom
            weight: 13
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Item {
                id: item
                implicitHeight: 50
                implicitWidth: 300
                LineEdit {
                    id: addr
                    implicitWidth: 200
                    text: dateAndTimeSettings.customAddr
                    placeholderText: qsTr("Required")
                    alertText: qsTr("The ntp server address cannot be empty")
                    anchors{
                        rightMargin: 10
                        right: editBtn.left
                        verticalCenter: parent.verticalCenter
                    }
                    onReadOnlyChanged: {
                        addr.background.visible = !addr.readOnly
                        addr.clearButton.visible = !addr.readOnly && text.length > 0
                        addr.focus = !addr.readOnly
                    }
                    onTextChanged: {
                        if (addr.showAlert && addr.text.trim().length > 0) {
                            addr.showAlert = false
                        }
                    }
                    Component.onCompleted: {
                        Qt.callLater( function(){ addr.forceActiveFocus() } )
                        addr.readOnly = text.length > 0
                    }
                }
                IconButton {
                    id: editBtn
                    enabled: addr.text.length > 0
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    icon {
                        name: addr.readOnly ? "edit" : "ok"
                        width: 16
                        height: 16
                    }
                    onClicked: {
                        if (addr.text.trim().length === 0) {
                            addr.showAlert = true
                            return
                        }

                        addr.showAlert = false

                        if (!addr.readOnly) {
                            dccData.ntpServerAddress = addr.text
                        }

                        addr.readOnly = !addr.readOnly
                    }
                }
            }
        }
        DccObject {
            visible: false // 暂时隐藏，会导致逻辑很复杂
            name: "12/24h"
            parentName: "dateTimeGroup"
            displayName: qsTr("Use 24-hour format")
            weight: 14
            backgroundType: DccObject.Normal
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
        parentName: "timeAndLang"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {
            id: view
        }

        DccRepeater {
            id: userTimezoneRepeater
            model: dccData.userTimezoneModel()
            delegate: ItemZoneComp {
                name: "userTimezoneItem" + index
                parentName: "timezoneGroup"
                displayName: model.display
                description: model.description
                weight: 20 + 10 * (index + 1)
                shift: model.shift
                zoneId: model.zoneId
            }
        }

        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }

    DccObject {
        id: systemTimezone
        name: "systemTimezone"
        parentName: "timezoneGroup"
        displayName: qsTr("system time zone")
        weight: 12
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: ComboBox {
            id: combo
            flat: true
            implicitWidth: 280
            model: dccData.zoneSearchModel()
            textRole: "display"
            displayText: dccData.timeZoneDispalyName
            hoverEnabled: true
            currentIndex: dccData.currentTimeZoneIndex

            popup: SearchableListViewPopup {
                id: searchView
                implicitWidth: combo.width
                delegateModel: combo.delegateModel
                maxVisibleItems: combo.maxVisibleItems
                highlightedIndex: combo.highlightedIndex
                onSearchTextChanged: {
                    let delegateModel = dccData.zoneSearchModel()
                    delegateModel.setFilterWildcard(searchView.searchText);
                }
            }

            onActivated: function (index) {
                let zoneId = currentValue["zoneId"]
                dccData.setSystemTimeZone(zoneId)
            }
        }
    }

    DccObject {
        name: "timezoneList"
        parentName: "timezoneGroup"
        displayName: qsTr("Timezone list")
        weight: 12
        backgroundType: DccObject.Normal
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
                    id: customLoader
                    active: false
                    sourceComponent: SearchableListViewPopup {
                        id: popup
                        implicitWidth: 280
                        delegateModel: DelegateModel {
                            model: dccData.zoneSearchModel()
                            delegate: MenuItem {
                                useIndicatorPadding: true
                                width: popup.width
                                text: model.display
                                highlighted: hovered
                                hoverEnabled: true
                                checkable: true
                                autoExclusive: true
                                onHoveredChanged: {
                                    if (hovered)
                                        popup.highlightedIndex = index
                                }
                                onCheckedChanged: {
                                    let zoneId = model.zoneId
                                    dccData.addUserTimeZoneById(zoneId)
                                    popup.close()
                                }
                            }
                        }
                        maxVisibleItems: 16
                        onSearchTextChanged: {
                            let delegateModel = dccData.zoneSearchModel()
                            delegateModel.setFilterWildcard(popup.searchText);
                        }
                        onClosed: {
                            customLoader.active = false
                        }
                    }
                    onLoaded: {
                        item.open()
                        item.x = addButton.implicitWidth - item.implicitWidth + 10
                    }
                }

                onClicked: {
                    customLoader.active = true
                }
            }
        }
    }

    component ItemZoneComp: DccObject {
        property int shift: 8
        property string zoneId
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
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
                    verticalCenter: itemDelegate.verticalCenter
                }
                background: null
                onClicked: {
                    console.log("need remove timezone", dccObj.displayName)
                    dccData.removeUserTimeZoneById(dccObj.zoneId)
                }
            }
            background: DccItemBackground {
                separatorVisible: true
            }
        }
    }
}
