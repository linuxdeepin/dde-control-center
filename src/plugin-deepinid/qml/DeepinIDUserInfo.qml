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
    page: DccSettingsView {}
    DccObject {
        name: "body"
        parentName: "deepinid/userinfo"
        weight: 10
        pageType: DccObject.Item

        DccObject {
            name: "title"
            parentName: "deepinid/userinfo/body"
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
                        font.pixelSize: 16
                        visible: !nameControl.editing
                        text: dccData.model.userName
                    }

                    D.ActionButton {
                        id: editButton
                        anchors.left: nameLabel.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: nameLabel.verticalCenter
                        focusPolicy: Qt.NoFocus
                        icon.name: "dcc-edit"
                        icon.width: DS.Style.edit.actionIconSize
                        icon.height: DS.Style.edit.actionIconSize
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
                        horizontalAlignment: Text.AlignHCenter
                        visible: nameControl.editing
                        text: dccData.model.userName

                        onTextEdited: {
                            // validtor can not paste invalid text..
                            var regex = /^[^:]{0,32}$/
                            if (!regex.test(text)) {
                                var filteredText = text
                                filteredText = filteredText.replace(":", "")

                                // 长度 32
                                filteredText = filteredText.slice(0, 32)
                                text = filteredText
                            }
                        }

                        onEditingFinished: {
                            nameControl.editing = false
                            dccData.worker.setFullName(text)
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
            parentName: "deepinid/userinfo/body"
            displayName: qsTr("Synchronization Service")
            weight: 20
        }

        DeepinIDSyncService{}

        DccTitleObject {
            name: "accountSecurityTitle"
            parentName: "deepinid/userinfo/body"
            displayName: qsTr("Account and Security")
            weight: 40
            visible: false
        }

        DeepinIDAccountSecurity{}
    }

    DccObject {
        name: "footer"
        parentName: "deepinid/userinfo"
        weight: 20
        pageType: DccObject.Item
        DccObject {
            name: "delete"
            parentName: "deepinid/userinfo/footer"
            weight: 10
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Sign out")
                onClicked: {
                    dccData.worker.logoutUser()
                }
            }
        }
        DccObject {
            // 按钮区域可加个空项处理右对齐问题
            name: "spacer"
            parentName: "deepinid/userinfo/footer"
            weight: 20
            pageType: DccObject.Item
            page: Item {
                Layout.fillWidth: true
            }
        }
        DccObject {
            name: "cancel"
            parentName: "deepinid/userinfo/footer"
            weight: 30
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Go to web settings")
                onClicked: {
                    dccData.worker.openWeb()
                }
            }
        }
    }
}
