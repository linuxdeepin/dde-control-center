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

        Connections {
            target: dccData
            function onLangListChanged(langlist) {
                languageListTiltle.addLanguages(langlist)
            }
        }

        function langExisted(displayName) {
            for (let i = 0; i < languageList.children.length; ++i) {
                if (languageList.children[i].displayName === displayName)
                    return true;
            }
            return false;
        }

        function addLanguages(langs) {
            for (let i = 0; i < langs.length; ++i) {
                if (langExisted(langs[i]))
                    continue

                var obj = languageItem.createObject(languageList)
                obj.name = "languageItem" + i
                obj.parentName = "languageList"
                obj.displayName = langs[i]
                obj.weight = 20 + 10 * (i + 1)
                DccApp.addObject(obj)
            }
        }

        Component {
            id: languageItem
            DccObject {
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
                        visible: itemDelegate.isCurrentLang || languageListTiltle.isEditing
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
                            if (!languageListTiltle.isEditing || itemDelegate.isCurrentLang)
                                return

                            console.log("need remove language", dccObj.displayName)
                            dccData.deleteLang(dccObj.displayName)
                            DccApp.removeObject(dccObj)
                            dccObj.destroy(1000)
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

        // 语言列表项
        DccObject {
            id: languageList
            name: "languageList"
            parentName: "langAndFormat"
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {
                Component.onCompleted: {
                    dccData.ensureLangModel();
                }
            }

            onParentItemChanged: item => { if (item) item.topPadding = 10 }
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
                    onSelectedLang: function(lang) {
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
                    onSelectedRegion: function(region) {
                        regionLabel.text = region
                        dccData.setRegion(region)
                    }
                    onActiveChanged: {
                        if (active) {
                            // regionWndow.currentIndex = dccData.currentRegionIndex()
                            // regionWndow.currentText = dccData.region()
                        }
                    }
                }

                onClicked: function(mouse) {
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
            implicitWidth: rowlayout.implicitWidth
            implicitHeight: rowlayout.implicitHeight
            RowLayout {
                id: rowlayout
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
                    onSelectedRegion: function(locale, lang) {
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

    // time/currency/number formats component
    Component {
        id: formatObject
        DccObject {
            id: obj
            property string textRole
            property var comboModel: dccData.availableFormats(modelIndex)
            property int modelIndex: 0
            property var callback
            hasBackground: true
            pageType: DccObject.Editor
            page: Item {
                id: item
                implicitHeight: 36
                implicitWidth: obj.comboModel.length > 1 ? 280 : 80

                ComboBox {
                    id: comboBox
                    visible: obj.comboModel.length > 1
                    flat: true
                    implicitWidth: 280
                    model: obj.comboModel
                    textRole: obj.textRole
                    currentIndex: dccData.currentFormatIndex(modelIndex)
                    onActivated: function (index) {
                        obj.callback(obj, index)
                    }
                }

                Label {
                    id: label
                    visible: obj.comboModel.length === 1
                    text: obj.comboModel[0]
                    anchors {
                        // anchors.verticalCenter: parent // TODO: not work(use topMargin)
                        top: parent.top
                        topMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }
                }

                Connections {
                    target: dccData

                    function resetModel() {
                        obj.comboModel = [""]
                        obj.comboModel = dccData.availableFormats(obj.modelIndex)
                        label.text = obj.comboModel[0]
                    }

                    function onCurrentLanguageAndRegionChanged() {
                        resetModel();
                    }
                    function onSymbolChanged(symbol) {
                        let oldIndex = comboBox.currentIndex
                        resetModel();
                        comboBox.currentIndex = oldIndex
                    }
                }
            }
        }
    }

    function addFormatsObject(names, startIndex, parentName, parent) {
        for (let i = 0; i < names.length; ++i) {
            let obj = formatObject.createObject(parent)
            obj.name = names[i]
            obj.parentName = parentName
            obj.weight = 10 * (i + 1)
            obj.displayName = names[i]
            obj.modelIndex = startIndex + i
            obj.callback = comboActivatedCallback
            DccApp.addObject(obj)
        }
    }

    function comboActivatedCallback(obj, index) {
        dccData.setCurrentFormat(obj.modelIndex, index);
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

        Component.onCompleted: {
            var names = [qsTr("Week"), qsTr("First day of week"), qsTr("Long date"), qsTr("Short date"), qsTr("Long time"), qsTr("Short time")]
            let startIndex = 0 // Week
            addFormatsObject(names, startIndex, name, timeFormats)
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

        Component.onCompleted: {
            var names = [qsTr("Currency symbol"), qsTr("Positive currency"), qsTr("Negative currency")]
            let startIndex = 6 // Currency
            addFormatsObject(names, startIndex, name, currencyFormats)
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

        Component.onCompleted: {
            var names = [qsTr("Decimal symbol"), qsTr("Digit grouping symbol"), qsTr("Digit grouping"), qsTr("Page size")]
            let startIndex = 9 // Week
            addFormatsObject(names, startIndex, name, timeFormats)
        }
    }
}
