// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 快捷键的一行（DccObject 化）。
// 由分类容器里的 DccRepeater 创建，model 是 ShortcutFilterModel（每分类一个）。
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

DccObject {
    id: row
    // model role（见 ShortcutListModel::roleNames）
    readonly property string shortcutId: model.id
    readonly property int shortcutType: model.type
    readonly property string shortcutName: model.display
    readonly property string command: model.command
    readonly property string accels: model.accels
    readonly property bool isCustom: model.isCustom

    // 显示用键位（抓键预览/冲突键会临时覆盖它）
    property var displayedKeys: model.keySequence
    // 自定义快捷键编辑对话框的命令（异步获取）
    property string dialogCommand: command

    // 父级传入
    property bool editing: false
    property int restoreToken: 0
    property var captureOwner: null

    // 行状态
    property bool capturing: false
    property bool submitting: false
    property string conflictMessage: ""
    property bool conflictVisible: false
    property bool conflictActions: true
    property string conflictAccels: ""
    property string pendingCommandId: ""
    property double pendingCommandSerial: 0

    name: shortcutId
    pageType: DccObject.Item
    displayName: shortcutName
    // 搜索过滤由本分类的 ShortcutFilterModel 承担，行本身不参与：
    // 模型没有 matchesSearch role，绑定它会让每次求值都报
    // "Unable to assign [undefined] to bool" 并拖慢整表重建。

    function matches(id, type) {
        return row.shortcutId === id && row.shortcutType === type
    }

    function restore() {
        captureFailureTimer.stop()
        row.releaseCapture()
        if (!row.submitting)
            row.displayedKeys = model.keySequence
        row.conflictVisible = false
        row.conflictMessage = ""
        row.conflictActions = true
        row.conflictAccels = ""
        row.captureOwner.conflictAccels = ""
        row.pendingCommandId = ""
        row.pendingCommandSerial = 0
        row.capturing = false
    }

    function releaseCapture(clearConflict = true, keepOwner = false) {
        if (row.captureOwner && row.captureOwner.activeRow === row) {
            dccData.endKeyCapture()
            if (!keepOwner)
                row.captureOwner.activeRow = null
        }
        if (clearConflict)
            dccData.clearPendingConflict(row.shortcutId, row.shortcutType)
    }

    function beginCapture(item) {
        if (row.submitting)
            return
        row.captureOwner.restoreActiveRow()
        row.captureOwner.isEditing = false
        row.captureOwner.activeRow = row
        row.capturing = true
        dccData.beginKeyCapture(item, row.shortcutId, row.shortcutType, true)
    }

    function prepareModification() {
        captureFailureTimer.stop()
        row.releaseCapture()
        row.capturing = false
        row.submitting = true
    }

    function startCapture() {
        captureFailureTimer.stop()
        row.conflictMessage = ""
        row.conflictVisible = false
        row.conflictActions = true
        row.conflictAccels = ""
        row.displayedKeys = []
    }

    function showConflict(accels, message) {
        row.displayedKeys = dccData.formatKeys(accels)
        row.conflictMessage = message
        row.conflictActions = true
        row.conflictVisible = true
        row.conflictAccels = accels
        row.captureOwner.conflictAccels = accels
    }

    function showFailure(message) {
        row.releaseCapture(true, true)
        row.capturing = false
        row.displayedKeys = model.keySequence
        row.conflictMessage = message
        row.conflictActions = false
        row.conflictVisible = true
        row.conflictAccels = ""
        row.captureOwner.conflictAccels = ""
        captureFailureTimer.restart()
    }

    function finishModification(accels) {
        captureFailureTimer.stop()
        row.releaseCapture(true)
        row.displayedKeys = dccData.formatKeys(accels)
        row.conflictVisible = false
        row.conflictMessage = ""
        row.conflictAccels = ""
        row.captureOwner.conflictAccels = ""
        row.capturing = false
    }

    function clearShortcut() {
        captureFailureTimer.stop()
        row.releaseCapture()
        row.capturing = false
        dccData.clearShortcut(row.shortcutId, row.shortcutType)
        row.displayedKeys = dccData.formatKeys("") // 乐观显示，服务端回灌后按模型刷新
        row.conflictVisible = false
        row.conflictMessage = ""
        row.conflictAccels = ""
        row.captureOwner.conflictAccels = ""
    }

    function modifyShortcut(accels) {
        if (accels.length > 0) {
            row.prepareModification()
            dccData.modifyShortcut(row.shortcutId, accels, row.shortcutType)
        }
    }

    function replaceShortcut(accels) {
        if (accels.length > 0) {
            captureFailureTimer.stop()
            row.releaseCapture(false)
            row.capturing = false
            row.submitting = true
            dccData.replaceShortcut(row.shortcutId, accels, row.shortcutType)
        }
    }

    onEditingChanged: {
        if (!editing)
            row.restore()
    }
    onRestoreTokenChanged: row.restore()

    // 只在捕获中或等待命令回复时连接。
    Connections {
        target: (row.capturing || row.submitting || row.pendingCommandId !== "") ? dccData : null

        function onRequestRestore(id, type) {
            if (row.capturing && row.matches(id, type))
                row.restore()
        }
        function onRequestClear(id, type) {
            if (row.capturing && row.matches(id, type))
                row.clearShortcut()
        }
        function onKeyCaptureStarted(id, type) {
            if (row.capturing && row.matches(id, type))
                row.startCapture()
        }
        function onKeyCaptureFailed(id, type, reason) {
            if (row.capturing && row.matches(id, type))
                row.showFailure(qsTranslate("Shortcuts", "Failed to start shortcut capture. Please try again."))
        }
        function onKeyCaptureTimedOut(id, type) {
            if (row.capturing && row.matches(id, type))
                row.showFailure(qsTranslate("Shortcuts", "Shortcut input timed out. Try again."))
        }
        function onKeyCaptureCanceled(id, type) {
            if (row.capturing && row.matches(id, type))
                row.restore()
        }
        function onInvalidShortcutCaptured(id, type) {
            if (row.capturing && row.matches(id, type))
                row.showFailure(qsTranslate("Shortcuts", "Invalid keyboard shortcut. Set a new one."))
        }
        function onKeyConflicted(id, type, oldAccels, newAccels, message, replaceable) {
            if ((!row.capturing && !row.submitting) || !row.matches(id, type))
                return
            row.submitting = false
            if (replaceable)
                row.showConflict(newAccels, message)
            else
                row.showFailure(message)
        }
        function onKeyDone(id, type, accels) {
            if (!row.capturing || !row.matches(id, type))
                return
            row.displayedKeys = dccData.formatKeys(accels)
            row.conflictVisible = false
            row.conflictMessage = ""
            row.modifyShortcut(accels)
        }
        function onKeyEvent(id, type, accels) {
            if (row.capturing && row.matches(id, type))
                row.displayedKeys = dccData.formatKeys(accels)
        }
        function onShortcutModificationFinished(id, type, accels, success) {
            if (!row.submitting || !row.matches(id, type))
                return
            row.submitting = false
            if (success)
                row.finishModification(accels)
            else
                row.showFailure(qsTranslate("Shortcuts", "Failed to save the shortcut. Please try again."))
        }
        function onShortcutCommandReady(id, command, available, requestSerial) {
            if (row.pendingCommandId !== id
                    || row.pendingCommandSerial !== requestSerial
                    || row.shortcutId !== id)
                return

            row.pendingCommandId = ""
            row.pendingCommandSerial = 0
            if (!available || !row.editing || !row.visibleToApp)
                return

            row.dialogCommand = command
            dialogLoader.active = true
        }
    }

    // 模型刷新后同步显示（命令式赋值会打断 displayedKeys 的绑定）。
    Connections {
        target: model
        function onKeySequenceChanged() {
            if (!row.capturing && !row.submitting)
                row.displayedKeys = model.keySequence
        }
    }

    page: ColumnLayout {
        // 旧版 ItemDelegate 的内容区左右各缩进 6px；这里显式还原，
        // 保证快捷键名称/键位与卡片边缘的距离和旧版一致。
        x: 6
        width: parent ? parent.width - 12 : implicitWidth
        spacing: 0

        // 与旧版 ItemDelegate 的行高对齐（内容 35 + 上下各 6）。
        Item {
            Layout.preferredHeight: 6
            Layout.fillWidth: true
        }

        KeySequenceDisplay {
            id: edit
            enabled: !row.submitting
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            Layout.bottomMargin: row.conflictVisible ? 8 : 0
            text: row.shortcutName
            keys: row.displayedKeys
            placeholderText: qsTranslate("Shortcuts", "please enter a new shortcut key")
            background.visible: row.conflictVisible
            backgroundColor: row.conflictVisible ? DS.Style.edit.alertBackground : DS.Style.keySequenceEdit.background
            showEditButtons: row.editing && row.isCustom
            showWarnning: row.accels.length > 0
                          && row.captureOwner.conflictAccels === row.accels

            onRequestKeys: {
                row.beginCapture(edit)
            }
            onRequestEditKeys: {
                if (dialogLoader.active)
                    return
                row.captureOwner.restoreActiveRow()
                row.pendingCommandId = row.shortcutId
                row.pendingCommandSerial = dccData.requestShortcutCommand(row.shortcutId)
            }
            onRequestDeleteKeys: {
                row.pendingCommandId = ""
                dccData.deleteCustomShortcut(row.shortcutId)
            }
        }

        Item {
            id: conflictText
            visible: row.conflictVisible
            implicitHeight: visible ? 20 : 0
            Layout.fillWidth: true
            Layout.rightMargin: 20

            RowLayout {
                width: parent.width
                spacing: 3
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Label {
                    id: conflictTextLabel
                    text: row.conflictMessage + (row.conflictActions ? "," : "")
                    elide: Text.ElideLeft
                    horizontalAlignment: Text.AlignRight
                    HoverHandler { id: conflictHandler }
                    ToolTip.visible: conflictHandler.hovered
                    ToolTip.text: row.conflictActions
                                  ? conflictTextLabel.text + clickLabel.text + " " + cancelLabel.text + " " + orLabel.text + " " + replaceLabel.text
                                  : conflictTextLabel.text
                    ToolTip.delay: 500
                    ToolTip.timeout: 4000
                }
                Label {
                    id: clickLabel
                    text: qsTranslate("Shortcuts", "Click")
                    visible: row.conflictActions
                }
                Label {
                    id: cancelLabel
                    text: qsTranslate("Shortcuts", "Cancel")
                    visible: row.conflictActions
                    color: palette.highlight
                    MouseArea {
                        anchors.fill: parent
                        onClicked: row.restore()
                    }
                }
                Label {
                    id: orLabel
                    text: qsTranslate("Shortcuts", "or")
                    visible: row.conflictActions
                }
                Label {
                    id: replaceLabel
                    text: qsTranslate("Shortcuts", "Replace")
                    visible: row.conflictActions
                    color: palette.highlight
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            const newAccels = row.conflictAccels
                            row.replaceShortcut(newAccels)
                        }
                    }
                }
            }
        }

        Item {
            Layout.preferredHeight: 6
            Layout.fillWidth: true
        }
    }

    Timer {
        id: captureFailureTimer
        interval: 20000
        repeat: false
        onTriggered: row.restore()
    }

    Loader {
        id: dialogLoader
        active: false
        sourceComponent: ShortcutSettingDialog {
            onCloseConfirmed: {
                dialogLoader.active = false
                row.restore()
            }
        }
        onLoaded: {
            item.keyId = row.shortcutId
            item.keyName = row.shortcutName
            item.cmdName = row.dialogCommand
            item.keySequence = row.displayedKeys
            if (row.displayedKeys.length > 0)
                item.saveKeys = row.displayedKeys
            item.accels = row.accels
            item.saveAccels = row.accels
            item.saveKeyName = row.shortcutName
            item.saveCmdName = row.dialogCommand
            item.show()
        }
    }

    Component.onDestruction: row.releaseCapture()
}
