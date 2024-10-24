// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
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
    parentName: "Keyboard"
    displayName: qsTr("Shortcuts")
    icon: "dcc_nav_keyboard" // TODO: shortcut icon required
    weight: parent.weight // 300
    hasBackground: false
    page: DccSettingsView {
    }

    DccObject {
        id: shortcutSettingsBody
        property bool isEditing: false
        property string conflictAccels
        name: "shortcutSettingsBody"
        parentName: "shortcutSettingsView"
        displayName: qsTr("Shortcuts body")
        weight: 30
        hasBackground: false
        pageType: DccObject.Item
        signal requestRestore

        page: ColumnLayout {
            spacing: 10
            Timer {
                id: timer
                interval: 100
                onTriggered: {
                    shortcutView.model.setFilterWildcard(searchEdit.text);
                }
            }

            D.SearchEdit {
                id: searchEdit
                Layout.topMargin: 10
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                placeholder: qsTr("Search shortcuts")
                onTextChanged: {
                    timer.start()
                }
                onEditingFinished: {
                    timer.start()
                }
                Component.onCompleted: {
                    // clear
                    shortcutView.model.setFilterWildcard("");
                }
            }

            ListView {
                id: shortcutView
                property Item editItem
                property Item conflictText
                clip: true
                interactive: false // 外层有滚动了，listview 就别滚了
                implicitHeight: contentHeight
                implicitWidth: 600
                Layout.fillWidth: true

                model: dccData.shortcutSearchModel()

                section.property: "section"
                section.criteria: ViewSection.FullString
                section.delegate: RowLayout {
                    width: ListView.view.width
                    height: childrenRect.height

                    required property string section

                    Label {
                        text: parent.section
                        font.bold: true
                        font.pointSize: 13
                        leftPadding: 20
                        bottomPadding: 10
                        topPadding: 16
                    }

                    D.Button {
                        id: button
                        visible: parent.section === qsTr("Custom")
                        checkable: true
                        checked: shortcutSettingsBody.isEditing
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.rightMargin: 10
                        text: shortcutSettingsBody.isEditing ? qsTr("done") : qsTr("edit")
                        font.pointSize: 13
                        background: null
                        textColor: D.Palette {
                            normal {
                                common: D.DTK.makeColor(D.Color.Highlight)
                                crystal: D.DTK.makeColor(D.Color.Highlight)
                            }
                        }
                        onCheckedChanged: {
                            shortcutSettingsBody.isEditing = button.checked

                            if (!shortcutView.editItem)
                                return
                            shortcutView.editItem.keys = shortcutView.editItem.keySequence
                            shortcutView.editItem.focus = false
                            shortcutView.conflictText.visible = false
                            shortcutView.editItem = null
                            shortcutView.conflictText = null
                        }
                    }
                }

                delegate: ItemDelegate {
                    id: editorDelegate
                    checkable: false
                    implicitWidth: ListView.view.width
                    topInset: 0
                    bottomInset: 0
                    backgroundVisible: true
                    Layout.fillWidth: true
                    corners: model.corners

                    background: DccItemBackground {
                        id: background
                        separatorVisible: true
                    }

                    contentItem: ColumnLayout {
                        KeySequenceDisplay {
                            id: edit
                            text: model.display
                            keys: model.keySequence
                            placeholderText: qsTr("Please enter a new shortcut")
                            background.visible: conflictText.visible
                            backgroundColor: conflictText.visible ? DS.Style.edit.alertBackground : DS.Style.keySequenceEdit.background
                            Layout.alignment: Qt.AlignRight
                            Layout.bottomMargin:  conflictText.visible ? 8 : 0
                            Layout.fillWidth: true
                            showEditButtons: shortcutSettingsBody.isEditing && model.isCustom
                            showWarnning: model.accels.length > 0 && shortcutSettingsBody.conflictAccels === model.accels

                            onRequestKeys: {
                                if (shortcutView.editItem) {
                                    shortcutView.editItem.restore()
                                    // return
                                }

                                edit.keys = ""
                                dccData.updateKey(model.id)
                                shortcutView.editItem = edit
                                shortcutView.conflictText = conflictText
                                shortcutSettingsBody.isEditing = false
                            }
                            onRequestEditKeys: {
                                dialogloader.active = true
                            }
                            onRequestDeleteKeys: {
                                console.log("onRequestDeleteKeys", model.id)
                                dccData.deleteCustomShortcut(model.id)
                            }

                            function modifyShortcut(accels) {
                                console.log("modifyShortcut", model.id, accels)
                                if (accels.length > 0)
                                    dccData.modifyShortcut(model.id, accels)
                            }

                            function restore() {
                                edit.keys = model.keySequence
                                conflictText.visible = false

                                shortcutSettingsBody.conflictAccels = ""
                                shortcutView.editItem = null
                                shortcutView.conflictText = null
                            }

                            Loader {
                                id: dialogloader
                                active: false
                                sourceComponent: ShortcutSettingDialog {
                                    onClosing: {
                                        shortcutSettingsBody.isEditing = false
                                        dialogloader.active = false
                                    }
                                }
                                onLoaded: {
                                    item.keyId = model.id
                                    item.keyName = model.display
                                    item.cmdName = model.command
                                    item.keySequence = model.keySequence
                                    item.accels = model.accels

                                    item.show()
                                }
                            }
                        }

                        Item {
                            id: conflictText
                            visible: false
                            implicitHeight: 20
                            implicitWidth: row.implicitWidth
                            Layout.alignment: Qt.AlignRight
                            Layout.rightMargin: 20

                            Row {
                                id: row
                                anchors.fill: parent
                                spacing: 3
                                Text {
                                    text: dccData.conflictText + ","
                                }
                                Text {
                                    text: qsTr("Click")
                                }
                                Text {
                                    text: qsTr("Cancel")
                                    color: palette.highlight
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            edit.restore()
                                        }
                                    }
                                }
                                Text {
                                    text: qsTr("or")
                                }
                                Text {
                                    text: qsTr("Replace")
                                    color: palette.highlight
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            edit.modifyShortcut(shortcutSettingsBody.conflictAccels)
                                            shortcutSettingsBody.conflictAccels = ""
                                        }
                                    }
                                }
                            }
                        }

                    }
                }

                function restoreShortcutView() {
                    if (!shortcutView.editItem)
                        return

                    shortcutView.editItem.restore()
                }

                Connections {
                    target: shortcutSettingsBody
                    function onRequestRestore() {
                        shortcutView.restoreShortcutView()
                    }
                }
                Connections {
                    target: dccData
                    function onRequestRestore() {
                        shortcutView.restoreShortcutView()
                    }
                    function onRequestClear() {
                        onRequestRestore()
                    }
                    function onKeyConflicted(oldAccels, newAccels) {
                        if (shortcutView.conflictText)
                            shortcutView.conflictText.visible = true

                        shortcutSettingsBody.conflictAccels = newAccels
                    }
                    function onKeyDone(accels) {
                        if (!shortcutView.editItem)
                            return
                        shortcutView.editItem.focus = false
                        shortcutView.editItem.keys = dccData.formatKeys(accels)
                        shortcutView.conflictText.visible = false

                        shortcutView.editItem.modifyShortcut(accels)

                        shortcutView.editItem = null
                        shortcutView.conflictText = null

                    }
                    function onKeyEvent(accels) {
                        if (!shortcutView.editItem)
                            return

                        shortcutView.editItem.focus = false
                        shortcutView.editItem.keys = dccData.formatKeys(accels)
                    }
                }
            }
        }
    }

    DccObject {
        name: "bottomAreaFoot"
        parentName: "shortcutSettingsView"
        displayName: qsTr("Shortcuts bottom area view1")
        weight: 40
        hasBackground: false
        pageType: DccObject.Item

        DccObject {
            name: "bottomAreaRestoreButton"
            parentName: "bottomAreaFoot"
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Restore default")
                onClicked: {
                    shortcutSettingsBody.isEditing = false
                    shortcutSettingsBody.requestRestore()

                    dccData.resetAllShortcuts()
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

                Loader {
                    id: loader
                    active: addButton.needShowDialog
                    sourceComponent: ShortcutSettingDialog {
                        id: shortcutSettingDialog
                        onClosing: {
                            addButton.needShowDialog = false
                            shortcutSettingsBody.conflictAccels = ""
                        }
                    }
                    onLoaded: {
                        item.show()
                    }
                }

                onClicked: {
                    shortcutSettingsBody.isEditing = false
                    shortcutSettingsBody.requestRestore()

                    addButton.needShowDialog = true
                }
            }
        }
    }
}
