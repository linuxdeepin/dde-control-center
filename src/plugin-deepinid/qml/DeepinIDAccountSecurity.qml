// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "wechatAccount"
        parentName: "deepinid/userinfo/body"
        weight: 50
        pageType: DccObject.Item
        visible: false
        page: DccGroupView {}

        DccObject {
            name: "wechatTitle"
            parentName: "deepinid/userinfo/body/wechatAccount"
            displayName: qsTr("Bind WeChat")
            description: qsTr("By binding WeChat, you can securely and quickly log in to your %1 ID and local accounts.").arg(dccData.editionName())
            weight: 10
            pageType: DccObject.Editor
        }

        DccObject {
            name: "wechatInfo"
            parentName: "deepinid/userinfo/body/wechatAccount"
            weight: 20
            pageType: DccObject.Item
            page: RowLayout {
                id: wechatLayout
                property bool isBindWechat: dccData.model.wechatName.length > 0

                Item {
                    implicitHeight: 40
                    Layout.leftMargin: 10
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    D.DciIcon {
                        id: wechatIcon
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        name: "dcc_secwechat"
                        sourceSize: Qt.size(24, 24)
                        opacity: wechatLayout.isBindWechat ? 1 : 0.6
                    }

                    Label {
                        text: wechatLayout.isBindWechat ? dccData.model.wechatName : qsTr("Unlinked")
                        anchors.left: wechatIcon.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignLeft
                        opacity: wechatLayout.isBindWechat ? 1 : 0.6
                    }
                }

                D.ToolButton {
                    text: wechatLayout.isBindWechat ? qsTr("Unbinding") : qsTr("Link")
                    visible: dccData.editionName() === "deepin" ? false : true
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
                        if (isBindWechat) {
                            unbindWechat.show()
                        } else {
                            bindWechat.show()
                        }
                    }
                }

                ConfirmManager {
                    id: unbindWechat
                    title: qsTr("Are you sure you want to unbind WeChat?")
                    message: qsTr("After unbinding WeChat, you will not be able to use WeChat to scan the QR code to log in to %1 ID or local account.").arg(dccData.editionName())
                    leftBtnText: qsTr("Let me think it over")
                    rightBtnText: qsTr("Unbinding")

                    onAccepted: {
                        dccData.worker.unBindPlatform()
                    }
                }

                ConfirmManager {
                    id: bindWechat

                    onAccepted: {
                        dccData.worker.bindAccount();
                    }
                }
            }
        }
    }

    DccObject {
        name: "localAccount"
        parentName: "deepinid/userinfo/body"
        weight: 60
        pageType: DccObject.Item
        visible: false
        page: DccGroupView {}

        DccObject {
            name: "localTitle"
            parentName: "deepinid/userinfo/body/localAccount"
            displayName: qsTr("Local Account Binding")
            description: qsTr("After binding your local account, you can use the following functions:").arg(dccData.editionName())
            weight: 10
            pageType: DccObject.Editor
        }

        DccObject {
            parentName: "deepinid/userinfo/body/localAccount"
            displayName: qsTr("WeChat Scan Code Login System")
            description: qsTr("Use WeChat, which is bound to your %1 ID, to scan code to log in to your local account.").arg(dccData.editionName())
            icon: "dcc_wechattip"
            weight: 20
            pageType: DccObject.Editor
        }

        DccObject {
            parentName: "deepinid/userinfo/body/localAccount"
            displayName: qsTr("Reset password via %1 ID").arg(dccData.editionName())
            description: qsTr("Reset your local password via %1 ID in case you forget it.").arg(dccData.editionName())
            icon: "dcc_uosidtip"
            weight: 30
            pageType: DccObject.Editor
        }

        DccObject {
            parentName: "deepinid/userinfo/body/localAccount"
            displayName: qsTr("To use the above features, please go to Control Center - Accounts and turn on the corresponding options.")
            weight: 40
            pageType: DccObject.Editor
        }
    }
}
