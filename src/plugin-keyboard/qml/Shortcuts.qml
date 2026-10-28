// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2.3
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

DccObject {
    id: shortcutSettingsView
    name: "shortcutSettingsView"
    parentName: "keyboard"
    displayName: qsTr("Shortcuts")
    description: qsTr("System shortcut, custom shortcut")
    icon: "keyboard_fn"
    weight: parent.weight // 300
    property int searchEditWidth: 600
    property var viewScrollbar: ScrollBar { }

    PropertyAnimation {
        id: scrollbarAnimation
        target: viewScrollbar
        property: "opacity"
        duration: 200
        from: 0
        to: 1
        onFinished: viewScrollbar.increase()
    }

    page: DccSettingsView {
        ScrollBar.vertical: viewScrollbar
    }

    Connections {
        target: shortcutSettingsView
        function onDeactive() {
            dccData.endKeyCapture()
            shortcutSettingsBody.restoreToken++
            shortcutSettingsBody.isEditing = false
        }
    }

    DccObject {
        id: shortcutSettingsBody
        property bool isEditing: false
        // 自增令牌：行监听它做统一回滚（恢复默认、失活、退出编辑模式）。
        property int restoreToken: 0
        // 正在抓键的行：同一时刻只允许一个行持有键盘捕获。
        property var activeRow: null
        property string conflictAccels: ""

        function restoreActiveRow() {
            // 行自己的 restore() 会把 activeRow 置空并结束捕获。
            if (activeRow)
                activeRow.restore()
            activeRow = null
        }
        name: "shortcutSettingsBody"
        parentName: "shortcutSettingsView"
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
        }

        // ---------------- 搜索框 ----------------
        DccObject {
            name: "shortcutSearchRow"
            parentName: "shortcutSettingsBody"
            weight: 1
            pageType: DccObject.Item
            page: Item {
                width: parent ? parent.width : implicitWidth
                implicitHeight: searchEdit.implicitHeight + 36

                D.SearchEdit {
                    id: searchEdit
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 31
                    placeholder: qsTr("Search shortcuts")
                    onTextChanged: searchTimer.start()
                    onEditingFinished: searchTimer.start()
                    Component.onCompleted: {
                        dccData.setShortcutSearchText("");
                        shortcutSettingsView.searchEditWidth = Qt.binding(function() { return width; });
                    }
                }

                Timer {
                    id: searchTimer
                    interval: 100
                    onTriggered: dccData.setShortcutSearchText(searchEdit.text)
                }
            }
        }

        // ---------------- 分类 + 行 ----------------
        // 每个分类生成两个兄弟对象：
        //   1) 组标题（独立一行，无卡片背景，对齐旧版 section header）
        //   2) 行容器（DccGroupView isGroup=true，把行拼成一张卡片）
        DccRepeater {
            model: dccData.shortcutCategoryModel()
            delegate: DccObject {
                id: categoryObj
                property string categoryKey: model.key
                property string categoryName: model.displayName
                property bool categoryIsCustom: model.isCustom
                property var rowModel: dccData.shortcutModelForCategory(categoryObj.categoryKey)

                // 组标题：独立于卡片，只有文字（Custom 组右侧带编辑按钮）。
                DccObject {
                    // 容器名 = 服务端分类 key + 后缀（与行名同一条规则：不做任何改写）；
                    // 这串进 DccObject 路径，改名要同步 modulepaths.json
                    name: categoryObj.categoryKey + "Title"
                    parentName: "shortcutSettingsBody"
                    weight: 10 + index * 100
                    visible: categoryObj.rowModel.count > 0
                    pageType: DccObject.Item
                    page: RowLayout {
                        width: parent ? parent.width : implicitWidth
                        height: implicitHeight

                        Label {
                            text: categoryObj.categoryName
                            font.bold: true
                            font.pointSize: 13
                            leftPadding: 20
                            bottomPadding: 4
                            topPadding: 16
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        D.Button {
                            id: editModeButton
                            focusPolicy: Qt.NoFocus
                            visible: categoryObj.categoryIsCustom
                            checkable: true
                            checked: shortcutSettingsBody.isEditing
                            text: shortcutSettingsBody.isEditing ? qsTr("done") : qsTr("edit")
                            font: D.DTK.fontManager.t8
                            background: null
                            textColor: D.Palette {
                                normal {
                                    common: D.DTK.makeColor(D.Color.Highlight)
                                    crystal: D.DTK.makeColor(D.Color.Highlight)
                                }
                            }
                            onCheckedChanged: {
                                shortcutSettingsBody.isEditing = editModeButton.checked
                                if (!editModeButton.checked)
                                    shortcutSettingsBody.restoreToken++
                            }
                        }
                    }
                }

                // 行卡片：isGroup=true 让行拼成一张卡片（首/中/尾圆角 + 分隔线）。
                DccObject {
                    id: rowsObj
                    name: categoryObj.categoryKey + "Rows"
                    parentName: "shortcutSettingsBody"
                    weight: 11 + index * 100
                    visible: categoryObj.rowModel.count > 0
                    pageType: DccObject.Item
                    page: DccGroupView {
                        width: parent ? parent.width : implicitWidth
                        spacing: 0
                        isGroup: true
                    }

                    DccRepeater {
                        model: categoryObj.rowModel
                        delegate: ShortcutRow {
                            parentName: rowsObj.name
                            weight: 10 + index * 10
                            editing: shortcutSettingsBody.isEditing
                            restoreToken: shortcutSettingsBody.restoreToken
                            captureOwner: shortcutSettingsBody
                        }
                    }
                }
            }
        }
    }

    DccObject {
        id: bottomAreaFoot
        name: "bottomAreaFoot"
        parentName: "shortcutSettingsView"
        weight: 40
        pageType: DccObject.Item
        property int restoreButtonWidth: DS.Style.button.width

        DccObject {
            name: "bottomAreaRestoreButton"
            parentName: "bottomAreaFoot"
            pageType: DccObject.Item
            page: Button {
                id: restoreButton
                text: qsTr("Restore default")
                implicitWidth: {
                    const totalPadding = leftPadding + rightPadding
                    const contentWidth = implicitContentWidth + totalPadding
                    const minWidth = DS.Style.button.width
                    const maxWidth = shortcutSettingsView.searchEditWidth / 2 - totalPadding
                    return Math.min(Math.max(contentWidth, minWidth), maxWidth)
                }
                
                Text {
                    id: restoreHiddenText
                    text: restoreButton.text
                    font: restoreButton.font
                    visible: false
                }
                
                ToolTip.visible: {
                    const contentWidth = restoreHiddenText.width + leftPadding + rightPadding
                    return width < contentWidth && hovered
                }
                ToolTip.text: text
                
                onClicked: {
                    shortcutSettingsBody.isEditing = false
                    shortcutSettingsBody.restoreToken++
                    dccData.resetAllShortcuts()
                }
                Component.onCompleted: {
                    bottomAreaFoot.restoreButtonWidth = Qt.binding(function() { return implicitWidth; })
                }
            }
        }

        DccObject {
            name: "bottomAreaSpacer"
            parentName: "bottomAreaFoot"
            pageType: DccObject.Item
            page: Item {
                Layout.fillWidth: true
            }
        }

        DccObject {
            name: "bottomAreaAddButton"
            parentName: "bottomAreaFoot"
            pageType: DccObject.Item
            page: Button {
                id: addButton
                property bool needShowDialog: false
                text: qsTr("Add custom shortcut")
                implicitWidth: {
                    const totalPadding = leftPadding + rightPadding
                    const contentWidth = implicitContentWidth + totalPadding
                    const minWidth = DS.Style.button.width
                    const maxWidth = shortcutSettingsView.searchEditWidth - bottomAreaFoot.restoreButtonWidth - 2*totalPadding - DS.Style.control.spacing
                    return Math.min(Math.max(contentWidth, minWidth), maxWidth)
                }

                Text {
                    id: hiddenText
                    text: addButton.text
                    font: addButton.font
                    visible: false
                }
                ToolTip.visible: {
                    const contentWidth = hiddenText.width + leftPadding + rightPadding
                    return width < contentWidth && hovered
                }
                ToolTip.text: text

                Loader {
                    id: loader
                    active: addButton.needShowDialog
                    sourceComponent: ShortcutSettingDialog {
                        id: shortcutSettingDialog
                        onCloseConfirmed: {
                            addButton.needShowDialog = false
                        }
                    }
                    onLoaded: {
                        item.show()
                    }
                }

                onClicked: {
                    shortcutSettingsBody.isEditing = false
                    shortcutSettingsBody.restoreToken++
                    addButton.needShowDialog = true
                }
            }
        }
    }
}
