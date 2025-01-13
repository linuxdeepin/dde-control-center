// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

Item {
    id: root

    property alias title: confirmDlg.title
    property alias message: confirmDlg.message
    property alias leftBtnText: confirmDlg.leftBtnText
    property alias rightBtnText: confirmDlg.rightBtnText

    signal accepted()

    function show() {
        if (title.length === 0 && message.length === 0) {
            if (dccData.worker.checkPasswdEmpty()) {
                loader.sourceComponent = registerDlg
            } else {
                loader.sourceComponent = verifyDlg
            }
            loader.active = true
        } else {
            confirmDlg.show()
        }
    }

    // 检查密码是否合法，要求至少包含字母和数字，且8-64个字符
    function checkPasswordValid(password) {
        let result = {
            isValid: true,
            message: ""
        }

        const hasLetter = /[a-zA-Z]/.test(password)
        const hasDigit = /\d/.test(password)
        if (!hasLetter || !hasDigit) {
            result.isValid = false
            result.message = qsTr("Password must contain numbers and letters")
            return result  
        }

        if (password.length < 8 || password.length > 64) {
            result.isValid = false
            result.message = qsTr("Password must be between 8 and 64 characters")
            return result
        }

        return result
    }    

    ConFirmDialog {
        id: confirmDlg

        onClosing: function (close) {
            confirmDlg.close()
        }

        onAccepted: {
            if (dccData.worker.checkPasswdEmpty()) {
                loader.sourceComponent = registerDlg
            } else {
                loader.sourceComponent = verifyDlg
            }
            loader.active = true
            confirmDlg.close()
        }
    }

    Loader {
        id: loader
        active: false
        onLoaded: function () {
            loader.item.show()
        }
    }

    Component {
        id: registerDlg

        RegisterDialog {
            onClosing: function (close) {
                loader.active = false
            }
            onRegisterPasswd: {
                dccData.worker.registerPasswd(passwd)
                root.accepted()
                loader.active = false
            }
        }
    }

    Component {
        id: verifyDlg

        VerifyDialog {
            onClosing: function (close) {
                loader.active = false
            }
            onVerifyPasswd: {
                root.accepted()
                loader.active = false
            }
            onForgetPasswd: {
                dccData.worker.openForgetPasswd()
            }
        }
    }
}
