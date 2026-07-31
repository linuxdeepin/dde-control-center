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
            shortcutView.restoreShortcutView()
            shortcutSettingsBody.conflictAccels = ""
            shortcutSettingsBody.isEditing = false
        }
    }

    DccObject {
        id: shortcutSettingsBody
        property bool isEditing: false
        property string conflictAccels
        name: "shortcutSettingsBody"
        parentName: "shortcutSettingsView"
        weight: 30
        pageType: DccObject.Item
        signal requestRestore

        page: ColumnLayout {
            spacing: 10
            Timer {
                id: timer
                interval: 100
                onTriggered: {
                    shortcutView.model.setFilterFixedString(searchEdit.text);
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
                    shortcutSettingsView.searchEditWidth = Qt.binding(function() { return width; });
                }
            }

            ListView {
                id: shortcutView
                property var editItem
                property var conflictText
                property string pendingCommandId
                property double pendingCommandSerial: 0
                clip: true
                interactive: false // 外层有滚动了，listview 就别滚了
                implicitHeight: contentHeight
                implicitWidth: 600
                Layout.fillWidth: true

                model: dccData.shortcutSearchModel()

                section.property: "sectionKey"
                section.criteria: ViewSection.FullString
                section.delegate: RowLayout {
                    width: ListView.view.width
                    height: childrenRect.height

                    required property string section

                    Label {
                        text: dccData.sectionDisplayName(parent.section)
                        font.bold: true
                        font.pointSize: 13
                        leftPadding: 20
                        bottomPadding: 10
                        topPadding: 16
                    }

                    D.Button {
                        id: button
                        focusPolicy: Qt.NoFocus
                        visible: parent.section === dccData.customCategoryKey()
                        checkable: true
                        checked: shortcutSettingsBody.isEditing
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.rightMargin: 10
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
                            shortcutSettingsBody.isEditing = button.checked
                            shortcutView.pendingCommandId = ""

                            if (!shortcutView.editItem)
                                return
                            shortcutView.editItem.restore()
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
                        backgroundType: DccObject.Normal
                    }

                    contentItem: ColumnLayout {
                        KeySequenceDisplay {
                            id: edit
                            property string dialogCommand: model.command
                            property string shortcutId: model.id
                            property int shortcutType: model.type
                            text: model.display
                            keys: model.keySequence
                            placeholderText: qsTr("please enter a new shortcut key")
                            background.visible: conflictText.visible
                            backgroundColor: conflictText.visible ? DS.Style.edit.alertBackground : DS.Style.keySequenceEdit.background
                            Layout.alignment: Qt.AlignRight
                            Layout.bottomMargin:  conflictText.visible ? 8 : 0
                            Layout.fillWidth: true
                            showEditButtons: shortcutSettingsBody.isEditing && model.isCustom
                            showWarnning: model.accels.length > 0 && shortcutSettingsBody.conflictAccels === model.accels

                            Connections{
                                target: model
                                function onKeySequenceChanged() {
                                    edit.keys= model.keySequence
                                }
                            }

                            onRequestKeys: {
                                if (shortcutView.editItem) {
                                    shortcutView.editItem.restore()
                                }

                                shortcutView.editItem = edit
                                shortcutView.conflictText = conflictText
                                shortcutSettingsBody.isEditing = false
                                dccData.beginKeyCapture(edit, model.id, model.type)
                            }
                            onRequestEditKeys: {
                                if (dialogloader.active)
                                    return

                                shortcutView.pendingCommandId = model.id
                                shortcutView.pendingCommandSerial = dccData.requestShortcutCommand(model.id)
                            }
                            onRequestDeleteKeys: {
                                console.log("onRequestDeleteKeys", model.id)
                                shortcutView.pendingCommandId = ""
                                dccData.deleteCustomShortcut(model.id)
                            }

                            function modifyShortcut(accels) {
                                console.log("modifyShortcut", model.id, accels, model.type)
                                if (accels.length > 0)
                                    dccData.modifyShortcut(model.id, accels, model.type)
                            }

                            function replaceShortcut(accels) {
                                if (accels.length > 0)
                                    dccData.replaceShortcut(model.id, accels, model.type)
                            }

                            function matches(id, type) {
                                return shortcutId === id && shortcutType === type
                            }

                            function startCapture() {
                                conflictText.message = ""
                                conflictText.showActions = true
                                conflictText.visible = false
                                keys = []
                            }

                            function showConflict(accels, message) {
                                keys = dccData.formatKeys(accels)
                                conflictText.message = message
                                conflictText.showActions = true
                                conflictText.visible = true
                                shortcutSettingsBody.conflictAccels = accels
                            }

                            function showFailure(message) {
                                keys = model.keySequence
                                conflictText.message = message
                                conflictText.showActions = false
                                conflictText.visible = true
                                shortcutSettingsBody.conflictAccels = ""
                            }

                            function finishModification(accels) {
                                keys = dccData.formatKeys(accels)
                                conflictText.visible = false
                                conflictText.message = ""
                                shortcutSettingsBody.conflictAccels = ""
                                shortcutView.editItem = null
                                shortcutView.conflictText = null
                            }

                            function clearShortcut() {
                                dccData.endKeyCapture()
                                dccData.clearShortcut(model.id, model.type)

                                keys = dccData.formatKeys("")
                                conflictText.visible = false
                                conflictText.message = ""

                                shortcutSettingsBody.conflictAccels = ""
                                shortcutView.editItem = null
                                shortcutView.conflictText = null
                            }

                            function restore() {
                                dccData.endKeyCapture()
                                edit.keys = model.keySequence
                                conflictText.visible = false
                                conflictText.message = ""
                                conflictText.showActions = true

                                shortcutSettingsBody.conflictAccels = ""
                                shortcutView.editItem = null
                                shortcutView.conflictText = null
                            }

                            Loader {
                                id: dialogloader
                                active: false
                                sourceComponent: ShortcutSettingDialog {
                                    onCloseConfirmed: {
                                        dccData.endKeyCapture()
                                        dialogloader.active = false
                                        shortcutView.pendingCommandId = ""

                                        conflictText.visible = false
                                        shortcutSettingsBody.conflictAccels = ""
                                        shortcutView.editItem = null
                                        shortcutView.conflictText = null
                                    }
                                }
                                onLoaded: {
                                    edit.restore()

                                    item.keyId = model.id
                                    item.keyName = model.display
                                    item.cmdName = edit.dialogCommand
                                    item.keySequence = model.keySequence
                                    if (model.keySequence.length > 0) {
                                        item.saveKeys = model.keySequence
                                    }
                                    item.accels = model.accels
                                    item.saveAccels = item.accels
                                    item.saveKeyName = item.keyName
                                    item.saveCmdName = item.cmdName

                                    item.show()
                                }
                            }
                            Connections {
                                target: dccData
                                function onShortcutCommandReady(id, command, available, requestSerial) {
                                    if (shortcutView.pendingCommandId !== id
                                            || shortcutView.pendingCommandSerial !== requestSerial
                                            || model.id !== id)
                                        return

                                    shortcutView.pendingCommandId = ""
                                    shortcutView.pendingCommandSerial = 0
                                    if (!available)
                                        return

                                    edit.dialogCommand = command
                                    dialogloader.active = true
                                }
                            }
                        }

                        Item {
                            id: conflictText
                            property string message: ""
                            property bool showActions: true
                            visible: false
                            implicitHeight: 20
                            implicitWidth: row.implicitWidth
                            Layout.alignment: Qt.AlignRight
                            Layout.rightMargin: 20
                            Layout.fillWidth: true

                            onVisibleChanged : {
                                if (visible && model.index === (shortcutView.count - 1)) {
                                    scrollbarAnimation.start()
                                }
                            }

                            RowLayout {
                                id: row
                                width: parent.width
                                spacing: 3
                                Item {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                }
                                Label {
                                    id: conflictTextLabel
                                    text: conflictText.message + (conflictText.showActions ? "," : "")
                                    elide: Text.ElideLeft
                                    horizontalAlignment: Text.AlignRight
                                    HoverHandler { id: conflictHandler }
                                    ToolTip.visible: conflictHandler.hovered
                                    ToolTip.text: conflictText.showActions
                                                  ? conflictTextLabel.text + clickLabel.text + " " + cancelLabel.text + " " + orLabel.text + " " + replaceLabel.text
                                                  : conflictTextLabel.text
                                    ToolTip.delay: 500
                                    ToolTip.timeout: 4000
                                }
                                Label {
                                    id: clickLabel
                                    text: qsTr("Click")
                                    visible: conflictText.showActions
                                }
                                Label {
                                    id: cancelLabel
                                    text: qsTr("Cancel")
                                    visible: conflictText.showActions
                                    color: palette.highlight
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            edit.restore()
                                        }
                                    }
                                }
                                Label {
                                    id: orLabel
                                    text: qsTr("or")
                                    visible: conflictText.showActions
                                }
                                Label {
                                    id: replaceLabel
                                    text: qsTr("Replace")
                                    visible: conflictText.showActions
                                    color: palette.highlight
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            var newAccels = shortcutSettingsBody.conflictAccels
                                            dccData.endKeyCapture()
                                            edit.replaceShortcut(newAccels)
                                        }
                                    }
                                }
                            }
                        }

                    }
                }

                function restoreShortcutView() {
                    shortcutView.pendingCommandId = ""

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
                    function onRequestRestore(id, type) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        shortcutView.restoreShortcutView()
                    }
                    function onRequestClear(id, type) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        shortcutView.editItem.clearShortcut()
                    }
                    function onKeyCaptureStarted(id, type) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        shortcutView.editItem.startCapture()
                    }
                    function onKeyCaptureFailed(id, type, reason) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        shortcutView.editItem.showFailure(qsTr("Failed to start shortcut capture. Please try again."))
                    }
                    function onKeyConflicted(id, type, oldAccels, newAccels, message, replaceable) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        if (replaceable)
                            shortcutView.editItem.showConflict(newAccels, message)
                        else
                            shortcutView.editItem.showFailure(message)
                    }
                    function onKeyDone(id, type, accels) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        shortcutView.editItem.keys = dccData.formatKeys(accels)
                        shortcutView.conflictText.visible = false
                        shortcutView.conflictText.message = ""
                        shortcutView.editItem.modifyShortcut(accels)
                    }
                    function onKeyEvent(id, type, accels) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        shortcutView.editItem.keys = dccData.formatKeys(accels)
                    }
                    function onShortcutModificationFinished(id, type, accels, success) {
                        if (!shortcutView.editItem || !shortcutView.editItem.matches(id, type))
                            return
                        if (success)
                            shortcutView.editItem.finishModification(accels)
                        else
                            shortcutView.editItem.showFailure(qsTr("Failed to save the shortcut. Please try again."))
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
                    shortcutSettingsBody.requestRestore()

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
