// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

ColumnLayout {
    id: pwdLayout
    property string userId
    property string name: dccData.userName(pwdLayout.userId)
    property bool currentPwdVisible: true
    Layout.fillWidth: true

    signal requestClose();

    FontMetrics {
        id: fm
    }

    function minWidth(font, text, width) {
        fm.font = font
        return Math.min(width, fm.advanceWidth(text) + 20)
    }

    function getPwdInfo() {
        var info = {
            "pwd": pwdContainter.eidtItems[0].text,
            "pwdRepeat": pwdContainter.eidtItems[1].text,
            "pwdHint": pwdContainter.eidtItems[2].text
        }
        if (currentPwd.visible)
            info["oldPwd"] = currentPwd.edit.text

        return info
    }

    function checkPassword() {
        return pwdContainter.checkPassword()
    }

    PasswordItem {
        id: currentPwd
        visible: pwdLayout.currentPwdVisible
        label.text: qsTr("Current password")
        edit.placeholderText: qsTr("Required")
        Layout.leftMargin: 0
        Layout.rightMargin: 16
        Layout.bottomMargin: 10

        Loader {
            id: safepageLoader
            active: false
            property string msg
            sourceComponent: ComfirmSafePage {
                msg: safepageLoader.msg
                onClosing: function () {
                    safepageLoader.active = false
                }
                onRequestShowSafePage: {
                    dccData.showDefender()
                }
            }
            onLoaded: function () {
                safepageLoader.item.show()
            }
        }

        Connections {
            target: dccData
            function onPasswordModifyFinished(id, code, msg) {
                if (id !== pwdLayout.userId)
                    return

                // no error, set pwd hint
                if (code === 0) {
                    if (pwdContainter.eidtItems[2])
                        dccData.setPasswordHint(pwdLayout.userId, pwdContainter.eidtItems[2].text)
                    pwdLayout.requestClose()
                    return
                }
                let info = dccData.checkPasswordResult(code, msg, pwdLayout.name, pwdContainter.eidtItems[0].text)
                // wrong password
                if (info["oldPwd"] !== undefined && currentPwd.visible) {
                    currentPwd.edit.showAlertText(info["oldPwd"])
                    return
                }

                // new password error
                if (info["pwd"] !== undefined)
                    pwdContainter.eidtItems[0].showAlertText(info["pwd"])
            }

            function onShowSafetyPage(msg) {
                if (!safepageLoader.active) {
                    safepageLoader.msg = msg
                    safepageLoader.active = true
                }
            }
        }
    }

    RowLayout {
        id: pwdIndicator
        spacing: 4
        Layout.alignment: Qt.AlignRight | Qt.AlignBottom
        Layout.rightMargin: pwdLayout.currentPwdVisible ? 30 : 20
        Label {
            id: pwdStrengthHintText
            text: ""
            Layout.rightMargin: 6
            visible: false // if need text set visible
        }
        Repeater {
            id: indicatorRepeater
            readonly property var defaultModel: [ palette.button, palette.button, palette.button ]
            model: defaultModel
            delegate: Rectangle {
                height: 4
                width: 10
                color: modelData
                radius: 2
            }
        }

        function updateIndicatorColors(level, colors) {
            // Note: slice() 拷贝一份 defaultModel 数组
            // 如果直接等号后面赋值操作将会修改 defaultModel （就算是 readonly 也会？）
            let model = indicatorRepeater.defaultModel.slice()
            for (let i = 0; i < level; ++i) {
                model[i] = colors[level - 1]
            }
            indicatorRepeater.model = model
        }

        function update(level) {
            if (level > 0) {
                var colors = [ "#FF5736", "#FFAA00", "#15BB18" ]
                pwdStrengthHintText.color = colors[level - 1]
                if (level === 1) {
                    pwdStrengthHintText.text = qsTr("Weak")
                } else if (level === 2) {
                    pwdStrengthHintText.text = qsTr("Medium")
                } else if (level === 3) {
                    pwdStrengthHintText.text = qsTr("Strong")
                } else {
                    pwdStrengthHintText.text = ""
                }
                updateIndicatorColors(level, colors)
            } else {
                pwdStrengthHintText.text = ""
                indicatorRepeater.model = indicatorRepeater.defaultModel
            }
        }
    }

    ListModel {
        id: passwordModel
        ListElement {
            name: qsTr("Password")
            placeholder: qsTr("Required")
            echoButtonVisible: true
        }
        ListElement {
            name: qsTr("Repeat Password")
            placeholder: qsTr("Required")
            echoButtonVisible: true
        }
        ListElement {
            name: qsTr("Password hint")
            placeholder: qsTr("Optional")
            echoButtonVisible: false
        }
    }

    Rectangle {
        id: pwdContainter
        property var eidtItems: []
        radius: 8
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
        Layout.rightMargin: pwdLayout.currentPwdVisible ? 10 : 0
        Layout.bottomMargin: 30
        implicitHeight: 150
        color: "transparent"

        function emptyCheck(edit) {
            if (edit.text.length < 1) {
                edit.showAlertText(qsTr("Password cannot be empty"))
                return false
            }

            return true
        }

        function checkPassword() {
            if (currentPwd.visible && !emptyCheck(currentPwd.edit))
                return false

            let edit0 = pwdContainter.eidtItems[0]
            let edit1 = pwdContainter.eidtItems[1]
            let edit2 = pwdContainter.eidtItems[2]
            if (!edit0 || !edit1 || !edit2)
                return true

            if (!emptyCheck(edit0))
                return false

            // password repeat test
            if (edit1.text.length < 1 || edit1.text !== edit0.text) {
                edit1.showAlertText(qsTr("Passwords do not match"))
                return false
            }

            // password notchanged test
            if (edit0.text.length < 1 || (currentPwd.visible && currentPwd.edit.text === edit0.text)) {
                edit0.showAlertText(qsTr("New password should differ from the current one"))
                return false
            }

            let alertText = ""
            // pwcheck verifyPassword
            alertText = dccData.checkPassword(pwdLayout.name, edit0.text)
            if (alertText.length > 0) {
                edit0.showAlertText(alertText)
                return false
            }
            alertText = dccData.checkPassword(pwdLayout.name, edit1.text)
            if (alertText.length > 0) {
                edit1.showAlertText(alertText)
                return false
            }

            // password hint test
            if (edit0.text.split('').filter(c => edit2.text.includes(c)).length > 0) {
                edit2.showAlertText(qsTr("The hint is visible to all users. Do not include the password here."))
                return false
            }

            return true
        }

        ColumnLayout {
            spacing: 0
            anchors.fill: parent
            Repeater {
                Layout.bottomMargin: 20
                model: passwordModel
                delegate: D.ItemDelegate {
                    Layout.fillWidth: true
                    backgroundVisible: false
                    checkable: false
                    implicitHeight: 50
                    leftPadding: pwdLayout.currentPwdVisible ? 0 : 10
                    rightPadding: 10

                    contentItem: PasswordItem {
                        label.text: model.name
                        edit {
                            placeholderText: model.placeholder
                            echoButtonVisible: model.echoButtonVisible
                        }
                        onTextChanged: function(text) {
                            if (index == 0) {
                                if (text.length > 0) {
                                    let lvl = dccData.passwordLevel(text)
                                    pwdIndicator.update(lvl)
                                } else {
                                    pwdIndicator.update(0)
                                }
                            }
                        }
                        Component.onCompleted: {
                            pwdContainter.eidtItems[index] = this.edit
                        }
                    }

                    background: DccItemBackground {
                        separatorVisible: true
                    }
                }
            }

            Label {
                text: qsTr("The hint is visible to all users. Do not include the password here.")
                wrapMode: Text.WordWrap
                Layout.preferredWidth: pwdLayout.minWidth(font, text, dialog.width - 20)
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                Layout.leftMargin: 0
                font: D.DTK.fontManager.t7
            }
        }
    }

    component PasswordItem : RowLayout {
        id: pwdItem
        property alias label: leftItem
        property alias edit: rightItem
        signal textChanged(string text)

        Label {
            id: leftItem
            Layout.preferredWidth: 120
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        D.PasswordEdit {
            id: rightItem
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            echoMode: echoButtonVisible ? TextInput.Password :  TextInput.Normal
            alertDuration: 3000
            onTextChanged: {
                if (showAlert)
                    showAlert = false

                pwdItem.textChanged(text)
            }

            function showAlertText(text) {
                rightItem.showAlert = false
                rightItem.showAlert = true
                rightItem.alertText = text
            }
        }
    }
}
