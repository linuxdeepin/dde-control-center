// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    id: loginMethodTitle
    property string userId
    name: "loginMethodTitle"
    // parentName: ""
    displayName: qsTr("Login method")
    weight: 38
    pageType: DccObject.Item
    page: Label {
        leftPadding: 5
        text: dccObj.displayName
        font {
            pointSize: 13
            bold: true
        }
    }
    onParentItemChanged: item => { if (item) item.topPadding = 10 }

    DccObject {
        name: loginMethodTitle.parentName + "loginMethod"
        parentName: loginMethodTitle.parentName
        description: qsTr("Password, wechat, biometric authentication, security key")
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

        DccRepeater {
            model: [qsTr("Password")/*, qsTr("Biometric authentication"), qsTr("Security key")*/]
            delegate: DccObject {
                name: loginMethodTitle.parentName + "loginMethodItem" + index
                parentName: loginMethodTitle.parentName + "loginMethod"
                displayName: modelData
                weight: 10 + 10 * index
            }
        }

        DccObject {
            name: loginMethodTitle.parentName + "PasswordLoginTitle"
            parentName: loginMethodTitle.parentName + "loginMethodItem" + 0
            displayName: qsTr("Password")
            weight: 12
            pageType: DccObject.Item
            page: RowLayout {
                Label {
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
            parentName: loginMethodTitle.parentName + "loginMethodItem" + 0
            description: qsTr("Password, wechat, biometric authentication, security key")
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                name: loginMethodTitle.parentName + "PasswordModify"
                parentName: passwordGroupView.name
                displayName: qsTr("Modify password")
                weight: 12
                pageType: DccObject.Item
                enabled: dccData.currentUserId() === loginMethodTitle.userId || !dccData.isOnline(loginMethodTitle.userId)
                page: Item {
                    implicitHeight: 40
                    RowLayout {
                        anchors.fill: parent
                        Label {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            Layout.leftMargin: 15
                            text: dccObj.displayName
                        }

                        Control {
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            Layout.rightMargin: 10
                            contentItem: D.IconLabel {
                                icon.name: "arrow_ordinary_right"
                            }
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
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
                }
            }
            DccObject {
                name: loginMethodTitle.parentName + "PasswordValidityDays"
                parentName: passwordGroupView.name
                displayName: qsTr("Validity days")
                weight: 12
                pageType: DccObject.Editor
                page: SpinBox {
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
                    }

                    onFocusChanged: {
                        if (!focus)
                            dccData.setPasswordAge(loginMethodTitle.userId, value)
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
