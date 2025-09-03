// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

DccObject {
    id: root

    name: "userinfo"
    parentName: "deepinid"
    pageType: DccObject.Item
    page: DccRightView {}

    DccObject {
        name: "title"
        parentName: "deepinid/userinfo"
        weight: 10
        pageType: DccObject.Item
        page: ColumnLayout {
            Rectangle {
                id: iconBackground
                width: 100
                height: 100
                Layout.alignment: Qt.AlignHCenter
                color: "transparent"

                Image {
                    id: image
                    anchors.fill: parent
                    source: dccData.model.avatar
                    sourceSize: Qt.size(80, 80)

                    layer.enabled: true
                    layer.effect: MultiEffect {
                        maskEnabled: true
                        maskSource: maskRect
                        maskThresholdMin: 0.5
                        maskSpreadAtMin: 1.0
                    }

                    Rectangle {
                        id: maskRect
                        anchors.fill: parent
                        radius: iconBackground.width / 2
                        layer.enabled: true
                        visible: false
                    }
                }
            }

            Label {
                id: userRegion
                Layout.fillWidth: true
                Layout.topMargin: 20
                horizontalAlignment: Text.AlignHCenter
                font: D.DTK.fontManager.t8
                text: dccData.model.region
            }

            Control {
                id: nameControl
                Layout.fillWidth: true
                property bool editing: false

                Label {
                    id: nameLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: -10
                    anchors.top: parent.top
                    horizontalAlignment: Text.AlignHCenter
                    visible: !nameControl.editing
                    font: D.DTK.fontManager.t6
                    text: dccData.model.userName
                }

                D.ActionButton {
                    id: editButton
                    anchors.left: nameLabel.right
                    anchors.leftMargin: 10
                    anchors.verticalCenter: nameLabel.verticalCenter
                    focusPolicy: Qt.NoFocus
                    width: 30
                    height: 30                        
                    icon.name: "dcc-edit"
                    icon.width: DS.Style.edit.actionIconSize
                    icon.height: DS.Style.edit.actionIconSize
                    hoverEnabled: true
                    background: Rectangle {
                        anchors.fill: parent
                        property D.Palette pressedColor: D.Palette {
                            normal: Qt.rgba(0, 0, 0, 0.2)
                            normalDark: Qt.rgba(1, 1, 1, 0.25)
                        }
                        property D.Palette hoveredColor: D.Palette {
                            normal: Qt.rgba(0, 0, 0, 0.1)
                            normalDark: Qt.rgba(1, 1, 1, 0.1)
                        }
                        radius: DS.Style.control.radius
                        color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                        border {
                            color: parent.palette.highlight
                            width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                        }
                    }
                    visible: !nameControl.editing
                    onClicked: {
                        nameControl.editing = true
                        nameEdit.text = nameLabel.text
                        nameEdit.selectAll()
                        nameEdit.forceActiveFocus()
                    }
                }

                D.LineEdit {
                    id: nameEdit
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    width: 240
                    visible: nameControl.editing
                    maximumLength: 32 // 限制昵称长度为32个字符
                    validator: RegularExpressionValidator { regularExpression: /^[^<>&'"\s]{0,32}$/ }  //昵称不能包含<、>、&、'、"和空格
                    alertDuration: 3000
                    font: D.DTK.fontManager.t6
                    text: visible ? dccData.model.userName : ""
                    activeFocusOnPress: false
                    onTextChanged: {
                        // 如果输入为空，需要弹出提示
                        if (nameEdit.text.length === 0) {
                            nameEdit.alertText = qsTr("The nickname must be 1~32 characters long")
                            nameEdit.showAlert = true
                        } else if (nameEdit.showAlert) {
                            nameEdit.showAlert = false
                        }
                    }

                    onEditingFinished: {
                        nameEdit.showAlert = false
                        nameControl.editing = false
                        if (nameEdit.text.length !== 0 && nameEdit.text !== dccData.model.userName) {
                            dccData.worker.setFullName(text)
                        }
                    }

                    Keys.onPressed: {
                        // TODO: 按下回车键时，把焦点转移到昵称标签上，避免鼠标点击空白时又触发一次editingFinished信号
                        if (event.key === Qt.Key_Return) {
                            nameLabel.forceActiveFocus(true);
                        }
                    }
                }
            }
        }

        onParentItemChanged: {
            if (parentItem) {
                parentItem.bottomPadding = 50
            }
        }
    }

    DccTitleObject {
        name: "syncServiceTitle"
        parentName: "deepinid/userinfo"
        displayName: qsTr("Synchronization Service")
        weight: 20
    }

    DeepinIDSyncService{}

    DccTitleObject {
        name: "accountSecurityTitle"
        parentName: "deepinid/userinfo"
        displayName: qsTr("Account and Security")
        weight: 40
        visible: false
    }

    DeepinIDAccountSecurity{}

    DccObject {
        name: "buttonGrp"
        parentName: "deepinid/userinfo"
        pageType: DccObject.Item
        weight: 60
        page: RowLayout {
            Button {
                text: qsTr("Sign out")
                onClicked: {
                    dccData.worker.logoutUser()
                }
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("Go to web settings")
                onClicked: {
                    dccData.worker.openWeb()
                }
            }
        }
    }
}
