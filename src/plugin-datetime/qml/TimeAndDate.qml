// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQml.Models 2.11
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.private 1.0 as P
import org.deepin.dtk.style 1.0 as DS

import ZoneInfoModel 1.0

// 时间和日期
DccObject {
    DccObject {
        id: dateTimeContent
        name: "dateTimeContent"
        parentName: "datetime"
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
        parentName: "datetime"
        weight: 12
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 10
                item.bottomPadding = 5
            }
        }

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

            onParentItemChanged: item => {
                if (item) {
                    item.rightPadding = DS.Style.comboBox.spacing
                }
            }

            page: Item {
                implicitHeight: 36
                implicitWidth: dccData.ntpEnabled ? 280 : 80

                ComboBox {
                    id: comboBox
                    property var serverList: dccData.ntpServerList
                    flat: true
                    padding: 0
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
                        if (dccData.ntpServerAddress.length > 0)
                            dccData.previousServerAddress = dccData.ntpServerAddress
                        return index < 0 ? serverList.length - 1 : index
                    }
                    onActivated: function (index) {
                        if (dccData.ntpServerAddress.length > 0)
                            dccData.previousServerAddress = dccData.ntpServerAddress

                        dateAndTimeSettings.showCustom = (serverList[index] === qsTr("Customize"))
                        if (dateAndTimeSettings.showCustom) {
                            let savedCustomServer = dccData.getCustomNtpServer()
                            if (savedCustomServer.length > 0) {
                                dateAndTimeSettings.customAddr = savedCustomServer
                                dccData.ntpServerAddress = savedCustomServer
                            } else if (dateAndTimeSettings.customAddr.length > 0) {
                                dccData.ntpServerAddress = dateAndTimeSettings.customAddr
                            } else {
                                dccData.ntpServerAddress = ""
                            }
                            return
                        }

                        dccData.ntpServerAddress = serverList[index]
                    }

                    Component.onCompleted: {
                        let text = qsTr("Customize")
                        if (!comboBox.serverList.includes(text))
                            comboBox.serverList.push(text)

                        let currentServer = dccData.ntpServerAddress
                        let index = comboBox.serverList.indexOf(currentServer)
                        if (index < 0) {
                            comboBox.currentIndex = comboBox.serverList.length - 1
                            dateAndTimeSettings.showCustom = true
                            dateAndTimeSettings.customAddr = currentServer
                        } else {
                            dateAndTimeSettings.showCustom = false
                            let savedCustomServer = dccData.getCustomNtpServer()
                            if (savedCustomServer.length > 0) {
                                dateAndTimeSettings.customAddr = savedCustomServer
                            }
                        }
                    }
                }

                Button {
                    id: settingsButton
                    visible: !dccData.ntpEnabled
                    anchors.fill: parent
                    property bool needShowDialog: false
                    text: qsTr("Settings")
                    implicitWidth: fm.advanceWidth(text) + 12
                    implicitHeight: 30
                    FontMetrics {
                        id: fm
                    }

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
                    if (dccData.previousServerAddress.length > 0) {
                        dccData.ntpServerAddress = dccData.previousServerAddress
                    }
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
                implicitHeight: 40
                implicitWidth: 300
                D.LineEdit {
                    id: addr
                    implicitWidth: 200
                    text: dateAndTimeSettings.customAddr
                    placeholderText: qsTr("Required")
                    alertText: qsTr("The ntp server address cannot be empty")
                    alertDuration: 3000
                    horizontalAlignment: background.visible ? TextInput.AlignLeft : TextInput.AlignRight
                    anchors{
                        rightMargin: 5
                        right: editBtn.left
                        verticalCenter: parent.verticalCenter
                    }
                    rightPadding: (!addr.readOnly && addr.text.length > 0 ? addr.clearButton.width : 5)
                    onReadOnlyChanged: {
                        addr.background.visible = !addr.readOnly
                        addr.clearButton.visible = !addr.readOnly && text.length > 0
                        addr.focus = !addr.readOnly
                    }
                    onTextChanged: {
                        if (addr.showAlert && addr.text.length > 0) {
                            addr.showAlert = false
                        }
                    }
                    Component.onCompleted: {
                        Qt.callLater( function(){ addr.forceActiveFocus() } )
                        addr.readOnly = text.length > 0
                    }
                }
                D.IconButton {
                    id: editBtn
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    hoverEnabled: true
                    background: Rectangle {
                        anchors.fill: parent
                        property D.Palette pressedColor: D.Palette {
                            normal: Qt.rgba(0, 0, 0, 0.2)
                            normalDark: Qt.rgba(1, 1, 1, 0.25)
                        }
                        property D.Palette hoveredColor: D.Palette {
                            normal: Qt.rgba(0, 0, 0, 0.1)
                            normalDark: Qt.rgba(1, 1, 1, 0.1)
                        }
                        radius: DS.Style.control.radius
                        color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                        border {
                            color: parent.palette.highlight
                            width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                        }
                    }
                    icon {
                        name: addr.readOnly ? "dcc-edit" : "inactive"
                        width: 16
                        height: 16
                    }
                    onClicked: {
                        if (addr.text.length === 0) {
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
        parentName: "datetime"
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

        onParentItemChanged: item => { if (item) item.topPadding = 5 }
    }

    DccObject {
        id: systemTimezone
        name: "systemTimezone"
        parentName: "timezoneGroup"
        displayName: qsTr("system time zone")
        weight: 12
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor

        onParentItemChanged: item => {
            if (item) {
                item.rightPadding = DS.Style.comboBox.spacing
            }
        }

        page: Item {
            id: systemTimezoneItem
            implicitWidth: rowlayout.implicitWidth
            implicitHeight: rowlayout.implicitHeight
            property var model: dccData.zoneSearchModel()
            property var currentIndex: dccData.currentTimeZoneIndex
            property string saveZoneId: ""
            RowLayout {
                id: rowlayout
                Label {
                    id: timezoneLabel
                    text: dccData.timeZoneDispalyName
                }
                D.IconLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
                    icon.name: "arrow_ordinary_down"
                    icon.palette: D.DTK.makeIconPalette(timezoneLabel.palette)
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent

                Component.onCompleted: {
                    if (dccData.currentTimeZoneIndex >= 0) {
                        let model = dccData.zoneSearchModel()
                        systemTimezoneItem.saveZoneId = model.data(model.index(systemTimezoneItem.currentIndex, 0), ZoneInfoModel.ZoneIdRole)
                    }
                }

                Connections {
                    target: dccData
                    function onCurrentTimeZoneIndexChanged() {
                        if (dccData.currentTimeZoneIndex >= 0) {
                            let model = dccData.zoneSearchModel()
                            systemTimezoneItem.saveZoneId = model.data(model.index(systemTimezoneItem.currentIndex, 0), ZoneInfoModel.ZoneIdRole)
                        }
                    }
                }

                SearchableListViewPopup {
                    id: timezoneWindow
                    highlightedIndex: systemTimezoneItem.currentIndex
                    maxVisibleItems: 13

                    delegateModel: DelegateModel {
                        model: systemTimezoneItem.model
                        delegate: D.MenuItem {
                            useIndicatorPadding: true
                            width: timezoneWindow.viewWidth
                            text: model.display
                            font: D.DTK.fontManager.t6
                            highlighted: ListView.isCurrentItem
                            hoverEnabled: true
                            checkable: true
                            autoExclusive: true
                            checked: model.zoneId === systemTimezoneItem.saveZoneId

                            contentItem: RowLayout {
                                spacing: 8
                                Item {
                                    Layout.preferredWidth: parent.parent.useIndicatorPadding ? 20 : 0
                                    Layout.preferredHeight: parent.height
                                    visible: parent.parent.useIndicatorPadding
                                }
                                Text {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter
                                    text: parent.parent.text
                                    font: D.DTK.fontManager.t6
                                    color: parent.parent.palette.windowText
                                    elide: Text.ElideRight
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                            onHoveredChanged: {
                                if (hovered && !ListView.view.keyboardScrolling) {
                                    timezoneWindow.setViewIndex(index)
                                }
                            }
                            onCheckedChanged: {
                                if (checked && model.zoneId != systemTimezoneItem.saveZoneId) {
                                    let zoneId = model.zoneId
                                    dccData.setSystemTimeZone(zoneId)
                                    timezoneWindow.close()
                                }
                            }
                        }
                    }

                    onSearchTextChanged: {
                        let delegateModel = dccData.zoneSearchModel()
                        delegateModel.setFilterWildcard(timezoneWindow.searchText)
                    }
                }

                onClicked: function (mouse) {
                    if (!timezoneWindow.isVisible()) {
                        // 保持当前选中项可见，但居中显示
                        timezoneWindow.highlightedIndex = systemTimezoneItem.currentIndex
                        timezoneWindow.show()
                        timezoneWindow.setPositionByItem(parent)
                    }
                }
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
                text: qsTr("Add")
                implicitHeight: 30
                implicitWidth: 60

                SearchableListViewPopup {
                    id: timezoneListWindow
                    delegateModel: DelegateModel {
                        model: dccData.zoneSearchModel()
                        delegate: D.MenuItem {
                            useIndicatorPadding: true
                            width: timezoneListWindow.viewWidth
                            text: model.display
                            font: D.DTK.fontManager.t6
                            highlighted: ListView.isCurrentItem
                            hoverEnabled: true
                            checkable: true
                            autoExclusive: true

                            contentItem: RowLayout {
                                spacing: 8
                                Item {
                                    Layout.preferredWidth: parent.parent.useIndicatorPadding ? 20 : 0
                                    Layout.preferredHeight: parent.height
                                    visible: parent.parent.useIndicatorPadding
                                }
                                Text {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter
                                    text: parent.parent.text
                                    font: D.DTK.fontManager.t6
                                    color: parent.parent.palette.windowText
                                    elide: Text.ElideRight
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                            onHoveredChanged: {
                                if (hovered && !ListView.view.keyboardScrolling) {
                                    timezoneListWindow.setViewIndex(index)
                                }
                            }
                            onCheckedChanged: {
                                if (checked) {
                                    let zoneId = model.zoneId
                                    dccData.addUserTimeZoneById(zoneId)
                                    timezoneListWindow.close()
                                }
                            }
                        }
                    }
                    maxVisibleItems: 13
                    onSearchTextChanged: {
                        let delegateModel = dccData.zoneSearchModel()
                        delegateModel.setFilterWildcard(searchText);
                    }
                }

                onClicked: {
                    if (!timezoneListWindow.isVisible()) {
                        timezoneListWindow.setPositionByItem(parent)
                        timezoneListWindow.highlightedIndex = 0
                        timezoneListWindow.show()
                    }
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
            id: itemZoneCompItemDelegate
            visible: dccObj
            hoverEnabled: true
            implicitHeight: Math.max(50, textColumn.implicitHeight + 2)
            icon.name: dccObj.icon
            checkable: false

            contentItem: Item {
                id: contentLayout
                Column {
                    id: textColumn
                    anchors {
                        left: parent.left
                        leftMargin: 50
                        verticalCenter: parent.verticalCenter
                    }
                    spacing: 2

                    Label {
                        id: display
                        text: dccObj.displayName
                        font: D.DTK.fontManager.t6
                        elide: Text.ElideRight
                    }
                    Label {
                        id: description
                        visible: text !== ""
                        font: D.DTK.fontManager.t10
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
                    left: itemZoneCompItemDelegate.left
                    leftMargin: 10
                    top: itemZoneCompItemDelegate.top
                    topMargin: (itemZoneCompItemDelegate.height - clock.height) / 2
                }
            }

            D.IconButton {
                id: removeButton
                visible: itemZoneCompItemDelegate.hovered
                icon.name: "dcc-delete"
                icon.width: 16
                icon.height: 16
                implicitWidth: 32
                implicitHeight: 32
                anchors {
                    right: itemZoneCompItemDelegate.right
                    rightMargin: 10
                    verticalCenter: itemZoneCompItemDelegate.verticalCenter
                }
                background: Rectangle {
                    property D.Palette pressedColor: D.Palette {
                        normal: Qt.rgba(0, 0, 0, 0.2)
                        normalDark: Qt.rgba(1, 1, 1, 0.25)
                    }
                    property D.Palette hoveredColor: D.Palette {
                        normal: Qt.rgba(0, 0, 0, 0.1)
                        normalDark: Qt.rgba(1, 1, 1, 0.1)
                    }
                    radius: DS.Style.control.radius
                    color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                    border {
                        color: parent.palette.highlight
                        width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                    }
                }
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
