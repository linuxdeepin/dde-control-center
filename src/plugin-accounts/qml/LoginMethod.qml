// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

DccTitleObject {
    id: loginMethodTitle
    property string userId
    name: "loginMethodTitle"
    // parentName: ""
    displayName: qsTr("Login method")
    weight: 38

    DccObject {
        name: loginMethodTitle.parentName + "loginMethod"
        parentName: loginMethodTitle.parentName
        description: qsTr("Password, wechat, biometric authentication, security key")
        canSearch: loginMethodTitle.canSearch
        weight: 40
        pageType: DccObject.Item
        page: DccGroupView {}

        // DccObject {
        //     name: loginMethodTitle.parentName + "WeChatScanQR"
        //     parentName: loginMethodTitle.parentName + "loginMethod"
        //     displayName: qsTr("WeChat scan QR code")
        //     weight: 12
        //     pageType: DccObject.Editor
        //     page: Switch {
        //     }
        // }

        DccObject {
            name: loginMethodTitle.parentName + "loginMethodItem" + "password"
            parentName: loginMethodTitle.parentName + "loginMethod"
            displayName: qsTr("Password")
            canSearch: loginMethodTitle.canSearch
            weight: 10 + 10
        }

        DccObject {
            name: loginMethodTitle.parentName + "PasswordLoginTitle"
            parentName: loginMethodTitle.parentName + "loginMethodItem" + "password"
            displayName: qsTr("Password")
            canSearch: loginMethodTitle.canSearch
            weight: 12
            pageType: DccObject.Item
            page: RowLayout {
                D.Label {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.leftMargin: 10
                    font.bold: true
                    font.pointSize: 14
                    text: dccObj.displayName
                }
            }
            onParentItemChanged: item => { if (item) item.topPadding = 10 }
        }

        DccObject {
            id: passwordGroupView
            name: loginMethodTitle.parentName + "PasswordGroupView"
            parentName: loginMethodTitle.parentName + "loginMethodItem" + "password"
            description: qsTr("Password, wechat, biometric authentication, security key")
            canSearch: loginMethodTitle.canSearch
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                name: loginMethodTitle.parentName + "PasswordModify"
                parentName: passwordGroupView.name
                displayName: dccData.currentUserId() === loginMethodTitle.userId ? qsTr("Modify password") : qsTr("Reset password")
                canSearch: loginMethodTitle.canSearch
                backgroundType: DccObject.ClickStyle
                weight: 12
                enabled: dccData.currentUserId() === loginMethodTitle.userId || (dccData.curUserIsSysAdmin() && !dccData.isOnline(loginMethodTitle.userId))
                pageType: DccObject.Editor
                page: D.IconLabel {
                    icon {
                        name: "arrow_ordinary_right"
                        palette: D.DTK.makeIconPalette(palette)
                    }
                    opacity: enabled ? 1 : 0.4
                }
                onActive: {
                    pmdLoader.active = true
                }

                Loader {
                    id: pmdLoader
                    active: false
                    sourceComponent: PasswordModifyDialog {
                        userId: loginMethodTitle.userId
                        onClosing: function (close) {
                            pmdLoader.active = false
                        }
                    }
                    onLoaded: function () {
                        pmdLoader.item.show()
                    }
                }
            }
            DccObject {
                name: loginMethodTitle.parentName + "PasswordValidityDays" + "biometric"
                parentName: passwordGroupView.name
                displayName: qsTr("Validity days")
                canSearch: loginMethodTitle.canSearch
                weight: 12
                pageType: DccObject.Editor
                page: D.SpinBox {
                    id: sbAge
                    from: 1
                    to: 99999
                    value: dccData.passwordAge(loginMethodTitle.userId)
                    editable: true
                    validator: RegularExpressionValidator {
                        regularExpression: /[\d]{1,5}/
                    }
                    textFromValue: function(value) {
                        return value > 99998 ? qsTr("Always") : value
                    }
                    valueFromText: function(text, locale) {
                        if (text === qsTr("Always"))
                            return 99999

                        return text
                    }
                    onValueChanged: function() {
                        timer.restart()
                        if (!ti.activeFocus)
                            ti.text = sbAge.textFromValue(sbAge.value, sbAge.locale)
                    }

                    onFocusChanged: {
                        if (!focus) {
                            sbAge.value = sbAge.valueFromText(ti.text, sbAge.locale)
                            dccData.setPasswordAge(loginMethodTitle.userId, value)
                        }
                    }

                    contentItem: TextInput {
                        id: ti
                        text: sbAge.textFromValue(sbAge.value, sbAge.locale)
                        color: sbAge.palette.text
                        readOnly: !sbAge.editable
                        validator: sbAge.validator
                        inputMethodHints: sbAge.inputMethodHints
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 10
                        maximumLength: 6
                        selectByMouse: true
                        renderType: Text.NativeRendering
                        onEditingFinished: sbAge.value = sbAge.valueFromText(text, sbAge.locale)
                        onTextChanged: {
                            if (text.length > 0 && text.charAt(0) === "0") {
                                var trimmed = text.replace(/^0+/, "")
                                text = trimmed
                            }
                        }
                        onActiveFocusChanged: {
                            if (activeFocus) {
                                text = String(sbAge.value)
                            } else {
                                text = sbAge.textFromValue(sbAge.value, sbAge.locale)
                            }
                        }
                    }

                    Timer {
                        id: timer
                        interval: 1000
                        onTriggered: {
                            dccData.setPasswordAge(loginMethodTitle.userId, sbAge.value)
                        }
                    }
                    Connections {
                        target: dccData
                        function onPasswordAgeChanged(userId, age) {
                            if (userId === loginMethodTitle.userId) {
                                sbAge.value = dccData.passwordAge(loginMethodTitle.userId)
                            }
                        }
                    }
                }
            }
        }
    }
}
