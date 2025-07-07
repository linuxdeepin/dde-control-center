// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

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
    property alias conflitedText : conflictText.text

    ColumnLayout {
        spacing: 10
        width: parent.width
        Label {
            Layout.alignment: Qt.AlignHCenter
            font {
                family: D.DTK.fontManager.t5.family
                pixelSize: D.DTK.fontManager.t5.pixelSize
                weight: Font.Bold
            }
            text: qsTr("Add custom shortcut")
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
                    keyFileDialog.open()
                }
            }
            FileDialog {
                id: keyFileDialog
                title: "Please choose a file"
                onAccepted: {
                    var path = keyFileDialog.selectedFile.toString();
                    // remove prefixed "file:///"
                    path = path.replace(/^(file:\/{3})/,"/");
                    commandEdit.text = path
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
            placeholderText: qsTr("Please enter a new shortcut")
            background.visible: edit.keys.length === 0 || conflictText.visible || showAlertColor
            backgroundColor: {
                if (edit.showAlertColor || conflictText.text.length > 0)
                    return DS.Style.edit.alertBackground
                else
                    return DS.Style.keySequenceEdit.background
            }
            onRequestKeys: {
                keys = []
                dccData.updateKey("", 1)
            }
        }

        Item {
            width: parent.width
            Text {
                id: conflictText
                elide: Text.ElideRight
                clip: true
                font: D.DTK.fontManager.t6
                visible: text.length > 0
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.topMargin: 20
            spacing: 10
            Button {
                Layout.bottomMargin: 14
                Layout.fillWidth: true
                font: D.DTK.fontManager.t6
                Layout.leftMargin: 4
                text: qsTr("Cancel")
                onClicked: {
                    ddialog.close()
                }
            }
            Button {
                Layout.bottomMargin: 14
                Layout.fillWidth: true
                Layout.rightMargin: 24
                font: D.DTK.fontManager.t6
                text: qsTr("Add")
                enabled: commandEdit.text.length > 0 && nameEdit.text.length >0
                onClicked: {
                    if (edit.keys[0] === qsTr("None")) {
                        edit.showAlertColor = true
                        return;
                    }

                    if (ddialog.keyId.length > 0)
                        dccData.modifyCustomShortcut(ddialog.keyId, nameEdit.text, commandEdit.text, edit.accels)
                    else
                        dccData.addCustomShortcut(nameEdit.text, commandEdit.text, edit.accels)

                    ddialog.close()
                }
            }
        }

        Connections {
            target: dccData
            function onRequestRestore() {
                edit.keys = ddialog.keySequence
                conflictText.text = ""
            }
            function onRequestClear() {
                onRequestRestore()
            }
            function onKeyConflicted(oldAccels, newAccels) {
                edit.accels = newAccels // 冲突也可以覆盖
                conflictText.text = dccData.conflictText + ", " + qsTr("Click Add to replace")
            }
            function onKeyDone(accels) {
                edit.keys = dccData.formatKeys(accels)
                edit.accels = accels
                conflictText.text = ""
            }
            function onKeyEvent(accels) {
                edit.showAlertColor = false
                edit.keys = dccData.formatKeys(accels)
            }
        }
    }
}
    
