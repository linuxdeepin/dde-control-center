// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

// 语言和地区
DccObject {
    // 语言列表抬头
    DccObject {
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
                font: D.DTK.fontManager.t3
                text: dccObj.displayName
            }
            Button {
                id: button
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.rightMargin: 10
                text: qsTr("edit")
                background: null
                textColor: D.Palette {
                    normal {
                        common: D.DTK.makeColor(D.Color.Highlight)
                        crystal: D.DTK.makeColor(D.Color.Highlight)
                    }
                }
            }
        }

        // 语言列表项
        DccObject {
            name: "languageList"
            parentName: "langAndFormat"
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {}
            onParentItemChanged: item => item ? item.topPadding = 10 : console.log("null parent..")

            DccObject {
                name: "language" + 1
                parentName: "languageList"
                weight: 10
                pageType: DccObject.Item
                page:  RowLayout {
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: 20
                        text: qsTr("简体中文")
                    }
                    D.IconButton {
                        implicitWidth: 36
                        background: null
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.rightMargin: 20
                        icon.name: "sp_ok"
                        icon.width: 20
                        icon.height: 20
                    }
                }
            }
            DccObject {
                name: "language" + 2
                parentName: "languageList"
                weight: 20
                pageType: DccObject.Item
                page:  RowLayout {
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: 20
                        text: qsTr("简体中文-键盘")
                    }
                    D.IconButton {
                        implicitWidth: 36
                        background: null
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.rightMargin: 20
                        icon.name: "list_delete"
                        icon.width: 20
                        icon.height: 20
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

        onParentItemChanged: item => item ? item.topPadding = 10 : console.log("null parent..")

        DccObject {
            name: "langItem0"
            parentName: "otherLanguagesTitle"
            displayName: qsTr("Other language")
            weight: 10
            hasBackground: true
            pageType: DccObject.Editor
            page: Button {
                implicitWidth: 80
                text: qsTr("add")
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
                font: D.DTK.fontManager.t4
            }
        }

        onParentItemChanged: item => item ? item.topPadding = 10 : console.log("null parent..")
    }

    // 区域格式
    DccObject {
        name: "regionAndFormat"
        parentName: "langAndFormat"
        weight: 50
        displayName: qsTr("Language and Retion")
        description: qsTr("根据语言和区域选择匹配的日期和时间格式")
        hasBackground: true
        pageType: DccObject.Editor
        page: ComboBox {
            id: combo
            flat: true
            implicitWidth: 280
            // model: dccData.searchModel()
            textRole: "display"
            // displayText: dccData.timeZoneDispalyName
            displayText: "简体中文（中国大陆）"
        }

        onParentItemChanged: item => item ? item.topPadding = 10 : console.log("null parent..")
    }
}
