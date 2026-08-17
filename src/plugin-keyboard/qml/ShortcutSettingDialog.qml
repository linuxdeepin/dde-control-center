// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Layouts
import org.deepin.dtk as D
import org.deepin.dtk.style as DS
import org.deepin.dcc

D.DialogWindow {
    id: ddialog
    width: 400
    minimumWidth: 400
    minimumHeight: 360
    maximumWidth: minimumWidth
    visible: true
    icon: "preferences-system"
    modality: Qt.WindowModal
    property string keyId
    property alias keyName: nameEdit.text
    property alias cmdName: commandEdit.text
    property alias keySequence: edit.keys
    property alias accels: edit.accels
    property alias conflitedText: conflictText.text
    property string saveKeyName: ""
    property string saveCmdName: ""
    property string saveAccels: ""
    property var saveKeys: [qsTr("None")]
    property bool nameExists: false
    property bool nameLengthExceeded: false
    property string lastAcceptedName: ""
    property var captureRestoreKeys: [qsTr("None")]
    property string captureRestoreAccels: ""
    property bool captureRestoreValid: false
    property bool pendingConflict: false
    property bool submitting: false
    property double pendingRequestId: 0
    signal closeConfirmed()

    onClosing: function(close) {
        if (ddialog.submitting) {
            close.accepted = false
            return
        }
        dccData.endKeyCapture()
        dccData.clearPendingConflict(ddialog.keyId, 1)
        ddialog.pendingConflict = false
        ddialog.closeConfirmed()
    }

    ColumnLayout {
        spacing: 10
        width: parent.width

        Component.onCompleted: {
            // 初始化时检查名称是否存在
            if (nameEdit.text.trim().length > 0) {
                ddialog.nameExists = dccData.isShortcutNameExists(nameEdit.text.trim(), ddialog.keyId);
            }
        }
        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font {
                family: D.DTK.fontManager.t5.family
                pixelSize: D.DTK.fontManager.t5.pixelSize
                weight: Font.Bold
            }
            text: ddialog.keyId.length > 0 ? qsTr("Change custom shortcut") : qsTr("Add custom shortcut")
        }

        Label {
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 4
            font: D.DTK.fontManager.t6
            text: qsTr("Name:")
        }

        D.LineEdit {
            id: nameEdit
            Layout.rightMargin: 20
            Layout.preferredWidth: parent.width
            font: D.DTK.fontManager.t6
            maximumLength: 129
            placeholderText: qsTr("Required")
            showAlert: ddialog.nameLengthExceeded || ddialog.nameExists
            alertText: ddialog.nameLengthExceeded
                       ? qsTr("The name cannot exceed 128 characters.")
                       : qsTr("The shortcut name is already in use. Choose a different name.")
            onTextChanged: {
                if (text.length > 128) {
                    text = ddialog.lastAcceptedName.length === 128
                           ? ddialog.lastAcceptedName
                           : text.substr(0, 128)
                    ddialog.nameLengthExceeded = true
                    nameLengthAlertTimer.restart()
                }
                ddialog.lastAcceptedName = text
                // 检查名称是否已存在（包括系统和自定义快捷键）
                if (text.trim().length > 0) {
                    ddialog.nameExists = dccData.isShortcutNameExists(text.trim(), ddialog.keyId);
                } else {
                    ddialog.nameExists = false;
                }
            }
            Timer {
                id: nameLengthAlertTimer
                interval: 2000
                repeat: false
                onTriggered: ddialog.nameLengthExceeded = false
            }
        }

        Label {
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 4
            font: D.DTK.fontManager.t6
            text: qsTr("Command:")
        }

        D.LineEdit {
            id: commandEdit
            Layout.rightMargin: 20
            Layout.preferredWidth: parent.width
            font: D.DTK.fontManager.t6
            clearButton.visible: false
            placeholderText: qsTr("Required")
            D.ActionButton {
                anchors {
                    right: commandEdit.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                icon {
                    name: "keyboard_add_file"
                    height: 20
                    width: 20
                }
                onClicked: {
                    keyFileDialog.open();
                }
            }
            FileDialog {
                id: keyFileDialog
                title: "Please choose a file"
                onAccepted: {
                    var path = keyFileDialog.selectedFile.toString();
                    // remove prefixed "file:///"
                    path = path.replace(/^(file:\/{3})/, "/");
                    commandEdit.text = path;
                }
            }
        }

        KeySequenceDisplay {
            id: edit
            property bool showAlertColor: false
            property string accels
            text: qsTr("Shortcut")
            keys: [qsTr("None")]
            Layout.topMargin: 10
            Layout.rightMargin: 20
            Layout.preferredWidth: parent.width
            placeholderText: qsTr("please enter a shortcut key")
            background.visible: edit.keys.length === 0 || conflictText.visible || showAlertColor
            backgroundColor: {
                if (edit.showAlertColor || conflictText.text.length > 0)
                    return DS.Style.edit.alertBackground;
                else
                    return DS.Style.keySequenceEdit.background;
            }
            onRequestKeys: {
                captureFailureTimer.stop()
                dccData.clearPendingConflict(ddialog.keyId, 1)
                ddialog.pendingConflict = false
                conflictText.text = ""
                if (!ddialog.captureRestoreValid) {
                    var restoreKeys = []
                    for (var index = 0; index < edit.keys.length; ++index)
                        restoreKeys.push(edit.keys[index])
                    ddialog.captureRestoreKeys = restoreKeys
                    ddialog.captureRestoreAccels = edit.accels
                    ddialog.captureRestoreValid = true
                }
                dccData.beginKeyCapture(edit, ddialog.keyId, 1, true)
            }
        }

        DccLabel {
            id: conflictText
            Layout.rightMargin: 20
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            clip: true
            font: D.DTK.fontManager.t6
            visible: text.length > 0
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.topMargin: 0
            spacing: 10
            Button {
                Layout.bottomMargin: 14
                Layout.fillWidth: true
                font: D.DTK.fontManager.t6
                Layout.leftMargin: 4
                text: qsTr("Cancel")
                enabled: !ddialog.submitting
                onClicked: {
                    ddialog.close()
                }
            }
            Button {
                Layout.bottomMargin: 14
                Layout.fillWidth: true
                Layout.rightMargin: 24
                font: D.DTK.fontManager.t6
                text: ddialog.keyId.length > 0 ? qsTr("Save") : qsTr("Add")
                enabled: !ddialog.submitting && commandEdit.text.length > 0
                         && nameEdit.text.length > 0 && !ddialog.nameExists
                onClicked: {
                    if (edit.keys.length === 0 || edit.keys[0] === qsTr("None")) {
                        edit.showAlertColor = true;
                        return;
                    }

                    conflictText.text = ""
                    ddialog.submitting = true
                    if (ddialog.pendingConflict) {
                        ddialog.pendingRequestId = dccData.replaceCustomShortcut(
                                    ddialog.keyId, nameEdit.text, commandEdit.text, edit.accels)
                    } else if (ddialog.keyId.length > 0) {
                        ddialog.pendingRequestId = dccData.modifyCustomShortcut(
                                    ddialog.keyId, nameEdit.text, commandEdit.text, edit.accels)
                    } else {
                        ddialog.pendingRequestId = dccData.addCustomShortcut(
                                    nameEdit.text, commandEdit.text, edit.accels)
                    }
                }
            }
        }

        Connections {
            target: dccData
            function onRequestRestore(id, type) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                captureFailureTimer.stop()
                dccData.endKeyCapture()
                if (ddialog.captureRestoreValid) {
                    edit.keys = ddialog.captureRestoreKeys
                    edit.accels = ddialog.captureRestoreAccels
                } else {
                    edit.keys = ddialog.saveKeys
                    edit.accels = ddialog.saveAccels
                }
                ddialog.captureRestoreValid = false
                dccData.clearPendingConflict(ddialog.keyId, 1)
                ddialog.pendingConflict = false
                conflictText.text = ""
                // 重置名称验证状态
                if (nameEdit.text.trim().length > 0) {
                    ddialog.nameExists = dccData.isShortcutNameExists(nameEdit.text.trim(), ddialog.keyId);
                } else {
                    ddialog.nameExists = false;
                }
            }
            function onRequestClear(id, type) {
                if (id === ddialog.keyId && type === 1)
                    onRequestRestore(id, type)
            }
            function onKeyCaptureStarted(id, type) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                ddialog.pendingConflict = false
                conflictText.text = ""
                edit.keys = []
            }
            function showCaptureFailure(message) {
                captureFailureTimer.restart()
                edit.keys = ddialog.captureRestoreKeys
                edit.accels = ddialog.captureRestoreAccels
                ddialog.captureRestoreValid = false
                dccData.clearPendingConflict(ddialog.keyId, 1)
                ddialog.pendingConflict = false
                conflictText.text = message
            }
            function onKeyCaptureFailed(id, type, reason) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                showCaptureFailure(qsTr("Failed to start shortcut capture. Please try again."))
            }
            function onKeyCaptureTimedOut(id, type) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                showCaptureFailure(qsTr("Shortcut input timed out. Try again."))
            }
            function onKeyCaptureCanceled(id, type) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                onRequestRestore(id, type)
            }
            function onInvalidShortcutCaptured(id, type) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                showCaptureFailure(qsTr("Invalid keyboard shortcut. Set a new one."))
            }
            function onKeyConflicted(id, type, oldAccels, newAccels, message, replaceable) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                captureFailureTimer.stop()
                if (!replaceable) {
                    showCaptureFailure(message)
                    return
                }
                ddialog.captureRestoreValid = false
                ddialog.pendingConflict = true
                edit.accels = newAccels;
                conflictText.text = message + ", "
                        + (ddialog.keyId.length > 0
                           ? qsTr("click Save to make this shortcut key effective")
                           : qsTr("click Add to make this shortcut key effective"));
            }
            function onKeyDone(id, type, accels) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                captureFailureTimer.stop()
                ddialog.captureRestoreValid = false
                dccData.clearPendingConflict(ddialog.keyId, 1)
                ddialog.pendingConflict = false
                edit.keys = dccData.formatKeys(accels);
                edit.accels = accels;
                conflictText.text = "";
            }
            function onKeyEvent(id, type, accels) {
                if (id !== ddialog.keyId || type !== 1)
                    return
                edit.showAlertColor = false;
                edit.keys = dccData.formatKeys(accels);
            }
            function onCustomShortcutOperationFinished(requestId, success, errorMessage) {
                if (!ddialog.submitting || requestId !== ddialog.pendingRequestId)
                    return

                ddialog.submitting = false
                ddialog.pendingRequestId = 0
                if (success) {
                    ddialog.close()
                    return
                }

                edit.keys = ddialog.saveKeys
                edit.accels = ddialog.saveAccels
                ddialog.captureRestoreValid = false
                dccData.clearPendingConflict(ddialog.keyId, 1)
                ddialog.pendingConflict = false
                conflictText.text = errorMessage.length > 0
                        ? errorMessage
                        : qsTr("Failed to save the shortcut. Please try again.")
                captureFailureTimer.restart()
            }
        }

        Timer {
            id: captureFailureTimer
            interval: 20000
            repeat: false
            onTriggered: {
                // The failure handler has already restored the shortcut. Only
                // clear the temporary alert and any stale capture state here.
                dccData.endKeyCapture()
                ddialog.captureRestoreValid = false
                dccData.clearPendingConflict(ddialog.keyId, 1)
                ddialog.pendingConflict = false
                conflictText.text = ""
            }
        }
    }
}
