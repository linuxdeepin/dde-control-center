// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0
import QtQml.Models

// 语言和区域
DccObject {
    id: langAndFormat

    property int localeStateChanged: 0
    property int localeStateChanging: 1
    // 正在设置语言
    property int localeStateSetLang: 1 << 1
    // 正在locale-gen
    property int localeStateGenLocale: 1 << 2

    FontMetrics {
        id: fm
    }
    // 语言列表抬头
    DccObject {
        id: languageListTiltle
        property bool isEditing: false
        name: "languageListTiltle"
        parentName: "langAndFormat"
        displayName: qsTr("Language")
        weight: 10
        pageType: DccObject.Item
        page: RowLayout {
            Label {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 12
                text: dccObj.displayName
                font.pixelSize: DTK.fontManager.t5.pixelSize
                font.weight: 500
                color: DTK.themeType === ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
            }

            Button {
                id: button
                checkable: true
                visible: langRepeater.count > 1
                font.pixelSize: DTK.fontManager.t8.pixelSize
                checked: languageListTiltle.isEditing
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.rightMargin: 10
                text: languageListTiltle.isEditing ? qsTr("done") : qsTr("edit")
                background: null
                enabled: dccData.langState === 0
                textColor: Palette {
                    normal {
                        common: DTK.makeColor(Color.Highlight)
                        crystal: DTK.makeColor(Color.Highlight)
                    }
                }
                onCheckedChanged: {
                    languageListTiltle.isEditing = button.checked
                }
            }
        }
        onParentItemChanged: item => { if (item) item.bottomPadding = 5 }

        // 语言列表项
        DccObject {
            id: languageList
            name: "languageList"
            parentName: "langAndFormat"
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {
                Component.onCompleted: {
                    dccData.ensureLangModel()
                }
            }

            onParentItemChanged: item => {
                if (item) {
                    item.topPadding = 5
                    item.bottomPadding = 3
                }
            }

            DccRepeater {
                id: langRepeater
                model: dccData.langList
                delegate: DccObject {
                    name: "languageItem" + index
                    parentName: "languageList"
                    displayName: modelData
                    weight: 20 + 10 * (index + 1)
                    backgroundType: DccObject.Normal
                    pageType: DccObject.Item
                    enabled: dccData.langState === 0 // language set finished
                    page: ItemDelegate {
                        id: itemDelegate
                        property bool isCurrentLang: dccData.currentLang === dccObj.displayName
                        visible: dccObj
                        hoverEnabled: true
                        implicitHeight: 40
                        icon.name: dccObj.icon
                        checkable: false

                        Loader {
                            id: langNameLoader
                            property string text: dccObj.displayName
                            property bool shouldSplit: text.split("-").length === 2
                            anchors {
                                left: itemDelegate.left
                                leftMargin: 12
                                top: itemDelegate.top
                                topMargin: (itemDelegate.height - height) / 2
                            }
                            sourceComponent: shouldSplit ? splitComponent : singleComponent

                            Component {
                                id: singleComponent
                                Label {
                                    text: langNameLoader.text
                                    elide: Text.ElideRight
                                    font: DTK.fontManager.t6
                                }
                            }

                            Component {
                                id: splitComponent
                                RowLayout {
                                    spacing: 0
                                    Label {
                                        text: langNameLoader.text.split("-")[0] || ""
                                        font: DTK.fontManager.t6
                                    }
                                    Label {
                                        text: "-"
                                        font: DTK.fontManager.t6
                                    }
                                    Label {
                                        text: langNameLoader.text.split("-")[1] || ""
                                        font: DTK.fontManager.t6
                                    }
                                }
                            }
                        }

                        IconButton {
                            id: removeButton
                            visible: (itemDelegate.isCurrentLang && dccObj.enabled) ||
                                     languageListTiltle.isEditing
                            icon.name: itemDelegate.isCurrentLang ? "item_checked" : "list_delete"
                            icon.width: 16
                            icon.height: 16
                            implicitWidth: 36
                            implicitHeight: 36
                            anchors {
                                right: itemDelegate.right
                                top: itemDelegate.top
                                topMargin: (itemDelegate.height - removeButton.height) / 2
                            }
                            background: null
                            onClicked: {
                                if (!languageListTiltle.isEditing
                                        || itemDelegate.isCurrentLang)
                                    return

                                dccData.deleteLang(dccObj.displayName)
                            }
                        }

                        Loader {
                            active: itemDelegate.isCurrentLang && !dccObj.enabled 
                                && dccData.langState & langAndFormat.localeStateSetLang
                            sourceComponent: BusyIndicator {
                                running: visible
                                implicitWidth: 20
                                implicitHeight: 20
                            }
                            onLoaded: {
                                item.parent = itemDelegate
                                item.anchors.right = itemDelegate.right
                                item.anchors.rightMargin = 10
                                item.anchors.top = itemDelegate.top
                                item.anchors.topMargin = (itemDelegate.height - implicitHeight) / 2
                            }
                        }

                        background: DccItemBackground {
                            separatorVisible: true
                        }

                        onClicked: {
                            if (languageListTiltle.isEditing)
                                return

                            dccData.setCurrentLang(dccObj.displayName)
                        }
                    }
                }
            }
        }
    }

    // 其他语言列表 +
    DccObject {
        name: "otherLanguagesTitle"
        parentName: "langAndFormat"
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => { if (item) item.topPadding = 3 }

        DccObject {
            name: "langItem0"
            parentName: "otherLanguagesTitle"
            displayName: qsTr("Other languages")
            weight: 10
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Button {
                implicitWidth: fm.advanceWidth(text) + fm.averageCharacterWidth * 2
                implicitHeight: 30
                text: qsTr("add")
                LangsChooserDialog {
                    id: dialogLoader
                    viewModel: dccData.langSearchModel()
                    onSelectedLang: function (lang) {
                        dccData.addLang(lang)
                    }
                }

                onClicked: {
                    dialogLoader.active = true
                    languageListTiltle.isEditing = false
                }
            }
            onParentItemChanged: item => {
                if (item) {
                    item.implicitHeight = 40
                    item.leftPadding = 7
                }
            }
        }
    }

    // 区域格式抬头
    DccObject {
        name: "langlistTiltle"
        parentName: "langAndFormat"
        displayName: qsTr("Region")
        weight: 40
        pageType: DccObject.Item
        page: RowLayout {
            Label {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 12
                text: dccObj.displayName
                font.pixelSize: DTK.fontManager.t5.pixelSize
                font.weight: 500
                color: DTK.themeType === ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
            }
        }

        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }

    // 地区
    DccObject {
        name: "regions"
        parentName: "langAndFormat"
        weight: 45
        displayName: qsTr("Area")
        description: qsTr("Operating system and applications may provide you with local content based on your country and region")
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: Item {
            implicitWidth: rowlayout.implicitWidth
            implicitHeight: rowlayout.implicitHeight
            RowLayout {
                id: rowlayout
                Label {
                    id: regionLabel
                    text: dccData.region
                }
                IconLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
                    icon.name: "arrow_ordinary_down"
                    icon.palette: DTK.makeIconPalette(regionLabel.palette)
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent

                RegionsChooserWindow {
                    id: regionWndow
                    viewModel: dccData.regionSearchModel()
                    currentIndex: dccData.currentRegionIndex
                    currentText: dccData.region
                    onSelectedRegion: function (region) {
                        regionLabel.text = region
                        dccData.setRegion(region)
                    }
                }

                onClicked: function (mouse) {
                    if (!regionWndow.isVisible()) {
                        regionWndow.mousePosition = mouseArea.mapToGlobal(mouse.x, mouse.y + rowlayout.implicitHeight)
                        regionWndow.show()
                    }

                    languageListTiltle.isEditing = false
                }
            }
        }

        onParentItemChanged: item => {
            if (item) {
                item.bottomInset = 3
                item.leftPadding = 7
            }
        }
    }

    // 区域格式
    DccObject {
        id: regionAndFormat
        name: "regionAndFormat"
        parentName: "langAndFormat"
        weight: 50
        displayName: qsTr("Region and format")
        description: qsTr("Operating system and applications may set date and time formats based on regional formats")
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor

        property bool localeRunning: dccData.langState & langAndFormat.localeStateChanging
        property bool localeGenRunning: dccData.langState & langAndFormat.localeStateGenLocale
        enabled: !localeRunning || localeGenRunning
        page: Item {
            id: regionAndFormatItem
            implicitWidth: regionAndFormat.localeGenRunning ? 36 : layout.implicitWidth
            implicitHeight: regionAndFormat.localeGenRunning ? 36 : layout.implicitHeight
            RowLayout {
                id: layout
                visible: !regionAndFormat.localeGenRunning
                Label {
                    id: currentLabel
                    text: dccData.currentLanguageAndRegion
                }
                IconLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
                    icon.name: "arrow_ordinary_right"
                    icon.palette: DTK.makeIconPalette(currentLabel.palette)
                }
            }

            RowLayout {
                anchors.fill: parent
                visible: regionAndFormat.localeGenRunning
                Layout.rightMargin: 10
                BusyIndicator {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    running: visible
                    implicitWidth: 20
                    implicitHeight: 20
                }
            }

            MouseArea {
                anchors.fill: parent
                enabled: !regionAndFormat.localeGenRunning
                RegionFormatDialog {
                    id: regionDialog
                    currentIndex: dccData.currentLanguageAndRegionIndex()
                    viewModel: dccData.langRegionSearchModel()
                    onSelectedRegion: function (locale, lang) {
                        dccData.setCurrentLocaleAndLangRegion(locale, lang)
                    }

                    Connections {
                        target: dccData
                        function onCurrentLanguageAndRegionChanged(currentLanguageAndRegion) {
                            regionDialog.currentIndex = dccData.currentLanguageAndRegionIndex()
                        }
                    }
                }

                onClicked: {
                    regionDialog.show()
                    languageListTiltle.isEditing = false
                }
            }
        }

        onParentItemChanged: item => {
            if (item) {
                item.topInset = 3
                item.leftPadding = 7
            }
        }
    }

    // 时间日期格式
    DccObject {
        id: timeFormats
        name: "timeFormats"
        parentName: "langAndFormat"
        weight: 60
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 5
                item.bottomPadding = 3
            }
        }

        DccRepeater {
            id: timeFormatsRepeater
            model: dccData.timeDateModel()
            delegate: DccObject {
                name: model.name
                parentName: "timeFormats"
                displayName: model.name
                weight: 10 * (index + 1)
                backgroundType: DccObject.Normal
                pageType: DccObject.Editor
                page: ComboLabel {
                    comboModel: model.values
                    comboCurrentIndex: model.current
                    onComboBoxActivated: function (idx) {
                        dccData.setCurrentFormat(model.indexBegin + index, idx)
                    }
                }
                onParentItemChanged: item => {
                    if (item) {
                        item.implicitHeight = 40
                        item.leftPadding = 7
                    }
                }
            }
        }
    }

    // 货币符号格式
    DccObject {
        id: currencyFormats
        name: "currencyFormats"
        parentName: "langAndFormat"
        weight: 70
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 3
                item.bottomPadding = 3
            }
        }

        DccRepeater {
            id: currencyRepeater
            model: dccData.currencyModel()
            delegate: DccObject {
                name: model.name
                parentName: "currencyFormats"
                displayName: model.name
                weight: 10 * (index + 1)
                backgroundType: DccObject.Normal
                pageType: DccObject.Editor
                page: ComboLabel {
                    comboModel: model.values
                    comboCurrentIndex: model.current
                    onComboBoxActivated: function (idx) {
                        dccData.setCurrentFormat(model.indexBegin + index, idx)
                    }
                }
                onParentItemChanged: item => {
                    if (item) {
                        item.implicitHeight = 40
                        item.leftPadding = 7
                    }
                }
            }
        }
    }

    // 数字格式符号
    DccObject {
        id: numberFormats
        name: "numberFormats"
        parentName: "langAndFormat"
        weight: 80
        pageType: DccObject.Item
        page: DccGroupView {}

        onParentItemChanged: item => { if (item) item.topPadding = 3 }

        DccRepeater {
            id: numRepeater
            model: dccData.decimalModel()
            delegate: DccObject {
                name: model.name
                parentName: "numberFormats"
                displayName: model.name
                weight: 10 * (index + 1)
                backgroundType: DccObject.Normal
                pageType: DccObject.Editor
                page: ComboLabel {
                    comboModel: model.values
                    comboCurrentIndex: model.current
                    onComboBoxActivated: function (idx) {
                        dccData.setCurrentFormat(model.indexBegin + index, idx)
                    }
                }
                onParentItemChanged: item => {
                    if (item) {
                        item.implicitHeight = 40
                        item.leftPadding = 7
                    }
                }
            }
        }
    }

    // Number example
    DccObject {
        name: "langlistTiltle"
        parentName: "langAndFormat"
        weight: 90
        pageType: DccObject.Item
        visible: dccData.numberExampleFormat !== ""
        page: RowLayout {
            Label {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 12
                text: dccData.numberExampleFormat
                font.pixelSize: DTK.fontManager.t10.pixelSize
                color: DTK.themeType === ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 0.5) : Qt.rgba(1, 1, 1, 0.5)
            }
        }

        onParentItemChanged: item => { if (item) item.topPadding = 0 }
    }
}
