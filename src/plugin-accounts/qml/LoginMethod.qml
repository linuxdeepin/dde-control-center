// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0

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
            model: [qsTr("Password"), qsTr("Biometric authentication")/*, qsTr("Security key")*/]
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
                            id: control
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            Layout.rightMargin: 10
                            contentItem: IconLabel {
                                icon.name: "arrow_ordinary_right"
                                icon.palette: DTK.makeIconPalette(control.palette)
                                icon.mode: control.ColorSelector.controlState
                                icon.theme: control.ColorSelector.controlTheme
                                opacity: enabled ? 1 : 0.4
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
                                onClosing: function
                                (close) {
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

        DccObject {
            name: loginMethodTitle.parentName + "BiometricAuthenticationTitle"
            parentName: loginMethodTitle.parentName + "loginMethodItem" + 1
            displayName: qsTr("Biometric Authentication")
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
            id: faceAuthenticationGroupView
            name: loginMethodTitle.parentName + "FaceAuthenticationGroupView"
            parentName: loginMethodTitle.parentName + "loginMethodItem" + 1
            displayName: qsTr("FaceAuthenticationGroupView")
            description: qsTr("Face Authentication Settings")
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                id: faceRepeaterHeaderView
                name: loginMethodTitle.parentName + "FaceRepeaterHeaderView"
                parentName: faceAuthenticationGroupView.name
                description: qsTr("Up to 5 face data entries can be recorded.")
                displayName: qsTr("Face")
                icon: "user_biometric_face"
                property bool faceVisible: false
                backgroundType: DccObject.Normal
                property real iconSize: 30
                weight: 20
                pageType: DccObject.Editor
                page: ToolButton {
                    implicitHeight: 50
                    icon.name: "arrow_ordinary_up"
                    icon.width: 12
                    icon.height: 12
                    rotation: faceRepeaterHeaderView.faceVisible ? 0 : 180

                    onClicked: {
                        faceRepeaterHeaderView.faceVisible = !faceRepeaterHeaderView.faceVisible
                    }

                    Behavior on rotation {
                        NumberAnimation {
                            duration: 100
                        }
                    }
                }

                Connections {
                    target: faceRepeaterHeaderView.parentItem
                    function onClicked() {
                        faceRepeaterHeaderView.faceVisible = !faceRepeaterHeaderView.faceVisible
                    }
                }
            }
            DccRepeater {
                id: faceRepeater
                model: faceRepeaterHeaderView.faceVisible ? dccData.faceModel.facesList : []
                delegate: DccObject {
                    name: modelData
                    parentName: faceAuthenticationGroupView.name
                    displayName: modelData
                    pageType: DccObject.Item
                    weight: 30 + index
                    page: RowLayout {
                        TextInput {
                            id: faceItem
                            text: modelData
                            font.family: "Source Han Sans SC"
                            font.pixelSize: 14
                            topPadding: 8
                            bottomPadding: 8
                            leftPadding: 12
                            rightPadding: 0
                            height: 36
                            focus: false
                            wrapMode: Text.NoWrap

                            readOnly: true
                            focusPolicy: Qt.NoFocus
                            onEditingFinished: {
                                focus = false
                                if (modelData !== faceItem.text) {
                                    dccData.renameFace(modelData, faceItem.text)
                                    // modelData = factItem.text
                                }
                            }
                            onFocusChanged: {
                                if (!focus)
                                    readOnly = true;
                            }
                            Keys.onEnterPressed: {
                                focus = false
                            }
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        ToolButton {
                            id: editButton
                            visible: hoverHandler.hovered
                            icon.name: "edit"
                            background: Rectangle {
                                color: "transparent"
                            }
                            onClicked: {
                                faceItem.readOnly = false
                                faceItem.focus = true
                            }
                        }
                        ToolButton {
                            id: deleteButton
                            visible: hoverHandler.hovered
                            icon.name: "delete"
                            background: Rectangle {
                                color: "transparent"
                            }
                            onClicked: {
                                dccData.removeFace(modelData)
                            }
                        }

                        HoverHandler {
                            id: hoverHandler
                        }
                    }
                }
            }

            DccObject {
                id: addFaceLabel
                name: loginMethodTitle.parentName + "FaceAuthentication"
                // Todo: Visibility not updated after facesList size changing
                visible: faceRepeaterHeaderView.faceVisible && dccData.faceModel.facesList.length < 5
                parentName: faceAuthenticationGroupView.name
                pageType: DccObject.Item
                weight: 50
                page: Item {
                    implicitHeight: 24
                    Label {
                        height: 36
                        Layout.topMargin: 8
                        Layout.bottomMargin: 8
                        text: qsTr("Add New Faces")
                        color: "#0058de"
                        leftPadding: 10
                        font.family: "Source Han Sans SC"
                        font.pixelSize: 14
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            addFaceDialogLoader.active = true
                            addFaceDialogLoader.item.show()
                        }
                        Loader {
                            id: addFaceDialogLoader
                            active: false
                            sourceComponent: AddFaceinfoDialog {
                                onClosing: function (close) {
                                    addFaceDialogLoader.active = false
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
