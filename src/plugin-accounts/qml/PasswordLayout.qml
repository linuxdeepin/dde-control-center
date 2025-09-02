// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

ColumnLayout {
    id: pwdLayout
    property string userId
    property string name: dccData.userName(pwdLayout.userId)
    property bool currentPwdVisible: true
    property string currentName: name
    Layout.fillWidth: true
    spacing: 0
    
    property int maxLabelWidth: 100

    signal requestClose()
    signal labelWidthCalculated()

    FontMetrics {
        id: fm
        font: D.DTK.fontManager.t6
    }

    function maxWidth(font) {
        fm.font = font
        var texts = []
        if (currentPwd.visible) {
            texts.push(currentPwd.label.text)
        }
        
        // 获取每个delegate中label的text
        for (var i = 0; i < pwdContainter.repeater.count; i++) {
            var delegateItem = pwdContainter.repeater.itemAt(i)
            if (delegateItem && delegateItem.contentItem) {
                texts.push(delegateItem.contentItem.label.text)
            }
        }
        
        var maxWidth = 0
        for (var i = 0; i < texts.length; i++) {
            var width = fm.advanceWidth(texts[i])
            if (width > maxWidth) {
                maxWidth = width
            }
        }
        var finalWidth = maxWidth > 110 ? 110 : maxWidth
        pwdLayout.maxLabelWidth = Math.ceil(finalWidth)
    }
    
    Component.onCompleted: {
        maxWidth(currentPwd.label.font)
        labelWidthCalculated()
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

    function showUserNameMatchPasswordAlert() {
        if (pwdContainter && pwdContainter.eidtItems && pwdContainter.eidtItems[0]) {
            pwdContainter.eidtItems[0].showAlertText(qsTr("Different from the username"))
        }
    }

    function playErrorSound() {
        dccData.playSystemSound(14)
    }

    PasswordItem {
        id: currentPwd
        visible: pwdLayout.currentPwdVisible
        label.text: qsTr("Current password")
        label.font: D.DTK.fontManager.t6
        edit.placeholderText: qsTr("Required")
        implicitHeight: 30
        Layout.leftMargin: 0
        Layout.rightMargin: -DS.Style.dialogWindow.contentHMargin
        Layout.bottomMargin: 0

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
        Layout.rightMargin: 0
        Layout.bottomMargin: 6
        Layout.topMargin: 20
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
                implicitHeight: 4
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
            name: qsTr("New password")
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
        property alias repeater: repeater
        radius: 8
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
        Layout.rightMargin: 0
        Layout.topMargin: 4 - DS.Style.dialogWindow.contentHMargin
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
            alertText = dccData.checkPassword(pwdLayout.currentName, edit0.text)
            if (alertText.length > 0) {
                edit0.showAlertText(alertText)
                return false
            }
            alertText = dccData.checkPassword(pwdLayout.currentName, edit1.text)
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
            id: pwdColumnLayout
            spacing: 10
            anchors.fill: parent
            Repeater {
                id: repeater
                Layout.bottomMargin: 20
                model: passwordModel
                delegate: D.ItemDelegate {
                    implicitWidth: pwdColumnLayout.width
                    backgroundVisible: false
                    checkable: false
                    implicitHeight: 30
                    leftPadding: pwdLayout.currentPwdVisible ? 0 : 10
                    rightPadding: 0

                    contentItem: PasswordItem {
                        label.text: model.name
                        label.font: D.DTK.fontManager.t6
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
                Layout.alignment: Qt.AlignLeft
                Layout.rightMargin: 0
                Layout.leftMargin: maxLabelWidth + 27
                Layout.preferredWidth: pwdLayout.minWidth(font, text, pwdColumnLayout.width - Layout.leftMargin)
                font: D.DTK.fontManager.t8
            }
        }
    }

    component PasswordItem : RowLayout {
        id: pwdItem
        property alias label: leftItem
        property alias edit: rightItem
        signal textChanged(string text)
        spacing: 10
        Layout.alignment: Qt.AlignVCenter

        Label {
            id: leftItem
            Layout.preferredHeight: 30
            font: D.DTK.fontManager.t6
            elide: Text.ElideRight
            Layout.preferredWidth: pwdLayout.maxLabelWidth
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            verticalAlignment: Text.AlignVCenter

            ToolTip.visible: leftItemHoverHandler.hovered && truncated
            ToolTip.text: text

            HoverHandler {
                id: leftItemHoverHandler
            }
        }

        D.PasswordEdit {
            id: rightItem
            Layout.preferredHeight: 30
            topPadding: 0
            bottomPadding: 0
            font: D.DTK.fontManager.t7
            canCopy: false
            canCut: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            verticalAlignment: TextInput.AlignVCenter
            echoMode: echoButtonVisible ? TextInput.Password :  TextInput.Normal
            alertDuration: 3000
            onTextChanged: {
                if (showAlert)
                    showAlert = false

                if (!echoButtonVisible && text.length > 14) {
                    rightItem.text = text.substring(0, 14)
                    playErrorSound()
                    return
                }
                
                pwdItem.textChanged(text)
            }

            onEditingFinished: {
                if (echoButtonVisible && pwdContainter.eidtItems[2] != rightItem) {
                    if (text === pwdLayout.currentName && text.length > 0) {
                        showAlertText(qsTr("Different from the username"))
                    }
                }
            }

            function showAlertText(text) {
                rightItem.showAlert = false
                rightItem.showAlert = true
                rightItem.alertText = text
            }
        }
    }
}
