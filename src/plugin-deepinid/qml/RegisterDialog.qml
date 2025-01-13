// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    icon: "dcc_union_id"
    width: 400
    modality: Qt.WindowModal

    signal registerPasswd(string passwd)

    ColumnLayout {
        width: parent.width
        spacing: 10
        Label {
            Layout.fillWidth: true
            font: D.DTK.fontManager.t5
            text: qsTr("Set a Password")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true
            height: 5
        }

        D.PasswordEdit {
            id: edit0
            Layout.fillWidth: true
            placeholderText: qsTr("8-64 characters")
            validator: RegularExpressionValidator {
                regularExpression: /^[a-zA-Z0-9~`!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?]+$/ // 允许的输入数字、字母和特殊字符
            }
            alertDuration: 3000
            onTextChanged: {
                if (showAlert)
                    showAlert = false
            }
        }

        D.PasswordEdit {
            id: edit1
            Layout.fillWidth: true
            placeholderText: qsTr("Repeat the password")
            validator: RegularExpressionValidator {
                regularExpression: /^[a-zA-Z0-9~`!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?]+$/ // 允许的输入数字、字母和特殊字符
            }
            alertDuration: 3000
            onTextChanged: {
                if (showAlert)
                    showAlert = false
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.fillWidth: true
            spacing: 10
            Button {
                text: qsTr("Cancel")
                Layout.fillWidth: true
                onClicked: {
                    close()
                }
            }
            D.RecommandButton {
                text: qsTr("Confirm")
                Layout.fillWidth: true
                onClicked: {
                    var result0 = checkPasswordValid(edit0.text)
                    if (!result0.isValid) {
                        edit0.showAlert = false
                        edit0.showAlert = true
                        edit0.alertText = result0.message
                        return
                    }

                    var result1 = checkPasswordValid(edit1.text)
                    if (!result1.isValid) {
                        edit1.showAlert = false
                        edit1.showAlert = true
                        edit1.alertText = result1.message
                        return
                    }

                    if(edit0.text != edit1.text) {
                        edit1.showAlert = false
                        edit1.showAlert = true
                        edit1.alertText = qsTr("Passwords don't match")
                        return;
                    }

                    dialog.registerPasswd(edit0.text)
                }
            }
        }
    }
}
