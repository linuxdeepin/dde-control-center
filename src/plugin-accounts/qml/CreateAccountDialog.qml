// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import QtQuick.Window
import QtQml.Models
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0
import AccountsController 1.0

D.DialogWindow {
    id: dialog
    width: 460
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    icon: "preferences-system"
    modality: Qt.WindowModal
    title: qsTr("Create a new account")

    signal accepted()

    ColumnLayout {
        id: mainLayout
        width: dialog.width - 20
        
        property int unifiedLabelWidth: -1
        spacing: 0
        
        FontMetrics {
            id: dialogFm
            font: D.DTK.fontManager.t6
        }
        
        function calculateUnifiedLabelWidth() {
            var dialogTexts = [accountTypeLabel.text]
            for (var i = 0; i < namesModel.count; i++) {
                dialogTexts.push(namesModel.get(i).name)
            }
            var maxDialogWidth = 0
            
            for (var i = 0; i < dialogTexts.length; i++) {
                var width = dialogFm.advanceWidth(dialogTexts[i])
                if (width > maxDialogWidth) {
                    maxDialogWidth = width
                }
            }
            
            var maxWidth = Math.max(maxDialogWidth, pwdLayout.maxLabelWidth)
            var finalWidth = maxWidth > 110 ? 110 : maxWidth
            unifiedLabelWidth = Math.ceil(finalWidth)
            
            pwdLayout.maxLabelWidth = Math.ceil(finalWidth)
        }
        
        function checkUserNamePasswordMatch() {
            var userName = namesContainter.eidtItems[0].text
            var password = pwdLayout.getPwdInfo()["pwd"]
            if (userName === password && userName.length > 0) {
                pwdLayout.showUserNameMatchPasswordAlert()
                return false
            }
            return true
        }
        
        Connections {
            target: pwdLayout
            function onLabelWidthCalculated() {
                mainLayout.calculateUnifiedLabelWidth()
            }
        }
        
        Connections {
            target: dccData
            function onAccountCreationFinished(resultType, message) {
                createButton.enabled = true
                
                switch (resultType) {
                case CreationResult.NoError:
                    dialog.accepted()
                    close()
                    break
                case CreationResult.UserNameError:
                case CreationResult.UnknownError:
                    let nameEdit = namesContainter.eidtItems[0]
                    if (nameEdit) {
                        nameEdit.showAlert = true
                        nameEdit.alertText = message
                    }
                    break
                case CreationResult.PasswordError:
                case CreationResult.PasswordMatchError:
                    if (pwdLayout.pwdContainter && pwdLayout.pwdContainter.eidtItems && pwdLayout.pwdContainter.eidtItems[0]) {
                        pwdLayout.pwdContainter.eidtItems[0].showAlertText(message)
                    }
                    break
                case CreationResult.Canceled:
                    break
                default:
                    console.warn("Unknown creation result type:", resultType, message)
                    break
                }
            }
        }
        
        Label {
            text: dialog.title
            font.bold: true
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.bottomMargin: 20
            font.pixelSize: accountTypeLabel.font.pixelSize
        }

        RowLayout {
            implicitHeight: 30
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.bottomMargin: 20
            Layout.leftMargin: 12 - DS.Style.dialogWindow.contentHMargin
            Layout.rightMargin: 6 - DS.Style.dialogWindow.contentHMargin
            spacing: 10

            Label {
                id: accountTypeLabel
                height: 30
                text: qsTr("Account type")
                Layout.preferredWidth: mainLayout.unifiedLabelWidth
                Layout.alignment: Qt.AlignVCenter
                font: D.DTK.fontManager.t6
                elide: Text.ElideRight
                
                ToolTip.visible: accountTypeHoverHandler.hovered && truncated
                ToolTip.text: text

                HoverHandler {
                    id: accountTypeHoverHandler
                }
            }

            ComboBox {
                id: userType
                implicitHeight: 30
                padding: 0
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                model: dccData.userTypes(true)
                font: D.DTK.fontManager.t7
            }
        }

        ListModel {
            id: namesModel
            ListElement {
                name: qsTr("UserName")
                placeholder: qsTr("Required")
            }
            ListElement {
                name: qsTr("FullName")
                placeholder: qsTr("Optional")
            }
        }

        Rectangle {
            id: namesContainter
            property var eidtItems: []
            radius: 8
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.leftMargin: 12 - DS.Style.dialogWindow.contentHMargin
            Layout.rightMargin: 6 - DS.Style.dialogWindow.contentHMargin
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            implicitHeight: 68
            color: "transparent"

            function checkNames() {
                let edit0 = namesContainter.eidtItems[0]
                let edit1 = namesContainter.eidtItems[1]
                if (!edit0 || !edit1)
                    return true
                let alertText = dccData.checkUsername(edit0.text)
                if (alertText.length > 0) {
                    // TODO: dtk fixme, showAlert tip timeout cannot show alertText again
                    edit0.showAlert = false
                    edit0.showAlert = true
                    edit0.alertText = alertText
                    return false
                }

                alertText = dccData.checkFullname(edit1.text)
                if (alertText.length > 0) {
                    edit1.showAlert = false
                    edit1.showAlert = true
                    edit1.alertText = alertText
                    return false
                }

                return true
            }

            ColumnLayout {
                spacing: 8
                Layout.topMargin: 0
                Layout.bottomMargin: 0
                anchors.fill: parent
                Repeater {
                    model: namesModel
                    delegate: D.ItemDelegate {
                        Layout.fillWidth: true
                        backgroundVisible: false
                        checkable: false
                        implicitHeight: 30
                        leftPadding: 0
                        rightPadding: 0

                        contentItem: RowLayout {
                            height: parent.height
                            spacing: 10
                            Label {
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                text: model.name
                                Layout.preferredWidth: mainLayout.unifiedLabelWidth
                                font: D.DTK.fontManager.t6
                                elide: Text.ElideRight

                                ToolTip.visible: nameHoverHandler.hovered && truncated
                                ToolTip.text: text

                                HoverHandler {
                                    id: nameHoverHandler
                                }
                            }
                            D.LineEdit {
                                id: edit
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                implicitHeight: 30
                                placeholderText: model.placeholder
                                alertDuration: 3000
                                // can not past
                                // validator: RegularExpressionValidator {
                                //     regularExpression: index ? /^[^:]{0,32}$/ : /[A-Za-z0-9-_]{0,32}$/
                                // }
                                onTextChanged: {
                                    if (showAlert)
                                        showAlert = false

                                    if (index === 0) {
                                        var charRegex = /^[A-Za-z0-9-_]*$/
                                        var lengthRegex = /^.{0,32}$/
                                        
                                        var hasInvalidChars = !charRegex.test(text)
                                        var isTooLong = !lengthRegex.test(text)
                                        
                                        if (hasInvalidChars || isTooLong) {
                                            var filteredText = text
                                            filteredText = filteredText.replace(/[^A-Za-z0-9-_]/g, "")
                                            filteredText = filteredText.slice(0, 32)
                                            text = filteredText
                                            
                                            if (isTooLong) {
                                                alertText = qsTr("Username cannot exceed 32 characters")
                                                showAlert = true
                                            } else if (hasInvalidChars) {
                                                alertText = qsTr("Username can only contain letters, numbers, - and _")
                                                showAlert = true
                                            }
                                        }
                                        pwdLayout.currentName = text
                                    } else {
                                        var colonRegex = /:/
                                        var lengthRegex = /^.{0,32}$/
                                        
                                        var hasColon = colonRegex.test(text)
                                        var isTooLong = !lengthRegex.test(text)
                                        
                                        if (hasColon || isTooLong) {
                                            var filteredText = text
                                            filteredText = filteredText.replace(":", "")
                                            filteredText = filteredText.slice(0, 32)
                                            text = filteredText
                                            
                                            if (isTooLong) {
                                                alertText = qsTr("Full name cannot exceed 32 characters")
                                                showAlert = true
                                            } else if (hasColon) {
                                                alertText = qsTr("Full name cannot contain colons")
                                                showAlert = true
                                            }
                                        }
                                    }
                                }
                                Component.onCompleted: {
                                    namesContainter.eidtItems[index] = this
                                }
                            }
                        }

                        background: DccItemBackground {
                            separatorVisible: true
                        }
                    }
                }
            }
        }

        PasswordLayout {
            id: pwdLayout
            currentPwdVisible: false
            Layout.leftMargin: 2 - DS.Style.dialogWindow.contentHMargin
            Layout.rightMargin: 6 - DS.Style.dialogWindow.contentHMargin
            Layout.topMargin: 0
            Layout.bottomMargin: 7
            name:  {
                let nameEdit = namesContainter.eidtItems[0]
                if (nameEdit === undefined)
                    return ""

                return nameEdit.text
            }
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 6
            Layout.leftMargin: 6 - DS.Style.dialogWindow.contentHMargin
            Layout.rightMargin: 6 - DS.Style.dialogWindow.contentHMargin

            Button {
                Layout.fillWidth: true
                text: qsTr("Cancel")
                font: D.DTK.fontManager.t7
                onClicked: {
                    close()
                }
            }
            D.RecommandButton {
                id: createButton
                Layout.fillWidth: true
                text: qsTr("Create account")
                font: D.DTK.fontManager.t7
                onClicked: {
                    if (!namesContainter.checkNames())
                        return

                    if (!pwdLayout.checkPassword())
                        return

                    if (!mainLayout.checkUserNamePasswordMatch())
                        return

                    var info = pwdLayout.getPwdInfo()
                    info["type"] = userType.currentIndex
                    info["name"] = namesContainter.eidtItems[0].text
                    info["fullname"] = namesContainter.eidtItems[1].text

                    dccData.addUser(info)
                    createButton.enabled = false
                }
            }
        }
    }
}
