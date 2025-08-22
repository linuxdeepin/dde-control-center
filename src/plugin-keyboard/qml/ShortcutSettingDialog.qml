// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

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
            Layout.alignment: Qt.AlignHCenter
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
            placeholderText: qsTr("Required")
            showAlert: ddialog.nameExists
            alertText: qsTr("The shortcut name is already in use. Choose a different name.")
            onTextChanged: {
                // 检查名称是否已存在（包括系统和自定义快捷键）
                if (text.trim().length > 0) {
                    ddialog.nameExists = dccData.isShortcutNameExists(text.trim(), ddialog.keyId);
                } else {
                    ddialog.nameExists = false;
                }
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
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }
                icon.name: "dde-file-manager"
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
                keys = [];
                dccData.updateKey(ddialog.keyId, 1);
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
                onClicked: {
                    if (ddialog.keyId.length > 0) {
                        dccData.modifyCustomShortcut(ddialog.keyId, ddialog.saveKeyName, ddialog.saveCmdName, ddialog.saveAccels);
                    }
                    ddialog.close();
                }
            }
            Button {
                Layout.bottomMargin: 14
                Layout.fillWidth: true
                Layout.rightMargin: 24
                font: D.DTK.fontManager.t6
                text: ddialog.keyId.length > 0 ? qsTr("Save") : qsTr("Add")
                enabled: commandEdit.text.length > 0 && nameEdit.text.length > 0 && !ddialog.nameExists
                onClicked: {
                    if (edit.keys[0] === qsTr("None")) {
                        edit.showAlertColor = true;
                        return;
                    }

                    if (ddialog.keyId.length > 0)
                        dccData.modifyCustomShortcut(ddialog.keyId, nameEdit.text, commandEdit.text, edit.accels);
                    else
                        dccData.addCustomShortcut(nameEdit.text, commandEdit.text, edit.accels);

                    ddialog.close();
                }
            }
        }

        Connections {
            target: dccData
            function onRequestRestore() {
                edit.keys = ddialog.saveKeys;
                conflictText.text = "";
                // 重置名称验证状态
                if (nameEdit.text.trim().length > 0) {
                    ddialog.nameExists = dccData.isShortcutNameExists(nameEdit.text.trim(), ddialog.keyId);
                } else {
                    ddialog.nameExists = false;
                }
            }
            function onRequestClear() {
                onRequestRestore();
            }
            function onKeyConflicted(oldAccels, newAccels) {
                edit.accels = newAccels; // 冲突也可以覆盖
                var actionText = ddialog.keyId.length > 0 ? qsTr("click Save to make this shortcut key effective") : qsTr("click Add to make this shortcut key effective");
                conflictText.text = dccData.conflictText + ", " + actionText;
            }
            function onKeyDone(accels) {
                edit.keys = dccData.formatKeys(accels);
                edit.accels = accels;
                conflictText.text = "";
            }
            function onKeyEvent(accels) {
                edit.showAlertColor = false;
                edit.keys = dccData.formatKeys(accels);
            }
        }
    }
}
