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
    // 语言列表抬头
    DccObject {
        id: languageListTiltle
        property bool isEditing: false
        name: "languageListTiltle"
        parentName: "langAndFormat"
        displayName: qsTr("Language list")
        weight: 10
        hasBackground: false
        pageType: DccObject.Item
        page: RowLayout {
            Label {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 20
                font: DTK.fontManager.t3
                text: dccObj.displayName
            }

            Button {
                id: button
                checkable: true
                checked: languageListTiltle.isEditing
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.rightMargin: 10
                text: languageListTiltle.isEditing ? qsTr("done") : qsTr("edit")
                background: null
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

            onParentItemChanged: item => { if (item) item.topPadding = 10 }

            DccRepeater {
                model: dccData.langList
                delegate: DccObject {
                    name: "languageItem" + index
                    parentName: "languageList"
                    displayName: modelData
                    weight: 20 + 10 * (index + 1)
                    hasBackground: true
                    pageType: DccObject.Item
                    page: ItemDelegate {
                        id: itemDelegate
                        property bool isCurrentLang: dccData.currentLang === dccObj.displayName
                        visible: dccObj
                        hoverEnabled: true
                        implicitHeight: 50
                        icon.name: dccObj.icon
                        checkable: false

                        Label {
                            id: langName
                            text: dccObj.displayName
                            elide: Text.ElideRight
                            anchors {
                                left: itemDelegate.left
                                leftMargin: 20
                                top: itemDelegate.top
                                topMargin: (itemDelegate.height - langName.height) / 2
                            }
                        }

                        IconButton {
                            id: removeButton
                            visible: itemDelegate.isCurrentLang
                                     || languageListTiltle.isEditing
                            icon.name: itemDelegate.isCurrentLang ? "sp_ok" : "list_delete"
                            icon.width: 24
                            icon.height: 24
                            implicitWidth: 36
                            implicitHeight: 36
                            anchors {
                                right: itemDelegate.right
                                rightMargin: 10
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

                        background: DccListViewBackground {
                            separatorVisible: true
                            highlightEnable: false
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

        onParentItemChanged: item => { if (item) item.topPadding = 10 }

        DccObject {
            name: "langItem0"
            parentName: "otherLanguagesTitle"
            displayName: qsTr("Other languages")
            weight: 10
            hasBackground: true
            pageType: DccObject.Editor
            page: Button {
                implicitWidth: 80
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
        }
    }

    // 区域格式抬头
    DccObject {
        name: "langlistTiltle"
        parentName: "langAndFormat"
        displayName: qsTr("Region and format")
        weight: 40
        hasBackground: false
        pageType: DccObject.Item
        page: RowLayout {
            Label {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 20
                text: dccObj.displayName
                font: DTK.fontManager.t4
            }
        }

        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }

    // 地区
    DccObject {
        name: "regions"
        parentName: "langAndFormat"
        weight: 45
        displayName: qsTr("Region")
        description: qsTr("Local content may be recommended based on your region.")
        hasBackground: true
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
                    icon.name: "arrow_ordinary_right"
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
                        regionWndow.show()
                    }

                    languageListTiltle.isEditing = false
                }
            }
        }

        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }

    // 区域格式
    DccObject {
        name: "regionAndFormat"
        parentName: "langAndFormat"
        weight: 50
        displayName: qsTr("Language and region")
        description: qsTr("Select matching date and time formats based on language and region")
        hasBackground: true
        pageType: DccObject.Editor
        page: Item {
            implicitWidth: layout.implicitWidth
            implicitHeight: layout.implicitHeight
            RowLayout {
                id: layout
                Label {
                    text: dccData.currentLanguageAndRegion
                }
                IconLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
                    icon.name: "arrow_ordinary_right"
                }
            }

            MouseArea {
                anchors.fill: parent
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

        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }

    Timer {
        id: timer
        interval: 100
        property DccRepeater repeater
        onTriggered: {
            if (repeater) {
                repeater.resetModel()
                repeater = null
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

        onParentItemChanged: item => { if (item) item.topPadding = 10 }

        DccRepeater {
            model: [ qsTr("Week"), qsTr("First day of week"), qsTr("Long date"),
                qsTr("Short date"), qsTr("Long time"), qsTr("Short time") ]
            delegate: DccObject {
                name: modelData
                parentName: "timeFormats"
                displayName: modelData
                weight: 10 * (index + 1)
                hasBackground: true
                pageType: DccObject.Editor
                page: ComboLabel {
                    comboModel: dccData.availableFormats(index)
                    comboCurrentIndex: dccData.currentFormatIndex(index)
                    onComboBoxActivated: function (idx) {
                        let startIndex = 0 // Week
                        dccData.setCurrentFormat(startIndex + index, idx)
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

        onParentItemChanged: item => { if (item) item.topPadding = 10 }

        DccRepeater {
            id: currencyRepeater
            property int startIndex: 6 // Currency
            model: [ qsTr("Currency symbol"), qsTr("Positive currency"), qsTr("Negative currency") ]
            delegate: DccObject {
                name: modelData
                parentName: "currencyFormats"
                displayName: modelData
                weight: 10 * (index + 1)
                hasBackground: true
                pageType: DccObject.Editor
                page: ComboLabel {
                    comboModel: dccData.availableFormats(index + currencyRepeater.startIndex)
                    comboCurrentIndex: dccData.currentFormatIndex(index + currencyRepeater.startIndex)
                    onComboBoxActivated: function (idx) {
                        dccData.setCurrentFormat(currencyRepeater.startIndex + index, idx)
                        timer.repeater = currencyRepeater
                        timer.start()
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

        onParentItemChanged: item => { if (item) item.topPadding = 10 }

        DccRepeater {
            id: numRepeater
            property int startIndex: 9 // number
            model: [ qsTr("Decimal symbol"), qsTr("Digit grouping symbol"),
                qsTr("Digit grouping"), qsTr("Page size") ]
            delegate: DccObject {
                name: modelData
                parentName: "numberFormats"
                displayName: modelData
                weight: 10 * (index + 1)
                hasBackground: true
                pageType: DccObject.Editor
                page: ComboLabel {
                    comboModel: dccData.availableFormats(index + numRepeater.startIndex)
                    comboCurrentIndex: dccData.currentFormatIndex(index + numRepeater.startIndex)
                    onComboBoxActivated: function (idx) {
                        dccData.setCurrentFormat(numRepeater.startIndex + index, idx)
                        timer.repeater = numRepeater
                        timer.start()
                    }
                }
            }
        }
    }
}
