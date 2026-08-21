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

    signal verifyPasswd()
    signal forgetPasswd()

    ColumnLayout {
        width: parent.width
        Label {
            Layout.fillWidth: true
            font: D.DTK.fontManager.t5
            text: qsTr("Security Verification")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Label {
            Layout.fillWidth: true
            font: D.DTK.fontManager.t6
            text: qsTr("The action is sensitive, please enter the login password first")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true
            height: 10
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

        D.ToolButton {
            id: btn
            Layout.alignment: Qt.AlignRight
            Layout.preferredHeight: 30
            text: qsTr("Forgot Password?")
            textColor: D.Palette {
                normal {
                    common: D.DTK.makeColor(D.Color.Highlight)
                }
                normalDark: normal
                hovered {
                    common: D.DTK.makeColor(D.Color.Highlight).lightness(+30)
                }
                hoveredDark: hovered
            }
            background: Item {}

            onClicked: {
                dialog.forgetPasswd()
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
            D.WarningButton {
                text: qsTr("Confirm")
                Layout.fillWidth: true
                onClicked: {
                    var result = checkPasswordValid(edit0.text)
                    if (!result.isValid) {
                        edit0.showAlert = false
                        edit0.showAlert = true
                        edit0.alertText = result.message
                        return
                    }

                    var checkRes = dccData.worker.checkPassword(edit0.text)
                    if (checkRes.isOk) {
                        dialog.verifyPasswd()
                    } else {
                        edit0.showAlert = false
                        edit0.showAlert = true
                        edit0.alertText = checkRes.errMsg
                    }
                }
            }
        }
    }
}
