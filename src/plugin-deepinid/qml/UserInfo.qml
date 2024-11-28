// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
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
                id: titleLayout
                Image {
                    id: userIcon
                    Layout.alignment: Qt.AlignHCenter
                    source: dccData.model.avatar
                    sourceSize: Qt.size(80, 80)
                    fillMode: Image.PreserveAspectFit
                    clip: true
                }

                // // fake round image
                // D.BoxShadow {
                //     id: boxShadow
                //     hollow: true
                //     anchors.fill: userIcon
                //     shadowBlur: 10
                //     spread: 20
                //     shadowColor: titleLayout.palette.window
                //     cornerRadius: boxShadow.width / 2
                // }

                Label {
                    id: userRegion
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: dccData.model.region
                }

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: dccData.model.displayName
                    }
                }
            }
        }

        DccObject {
            name: "syncService"
            parentName: "deepinid/userinfo/body"
            weight: 15
            pageType: DccObject.Item
            page: Rectangle {
                radius: 20
                implicitHeight: 300
                color: "#FFFFFF"

                Image {
                    id: waitImg
                    anchors.verticalCenterOffset: -20
                    anchors.centerIn: parent
                    source: "qrc:/icons/deepin/builtin/icons/waitting.svg"
                    sourceSize: Qt.size(40, 40)
                    fillMode: Image.PreserveAspectFit
                    clip: true
                }

                Label {
                    anchors.top: waitImg.bottom
                    anchors.topMargin: 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("The relevant functions are still under development, so stay tuned")
                }
            }
        }

        DccTitleObject {
            name: "syncServiceTitle"
            parentName: "deepinid/userinfo/body"
            displayName: qsTr("Synchronization Service")
            weight: 20
            visible: false
        }

        DccObject {
            name: "syncService"
            parentName: "deepinid/userinfo/body"
            weight: 30
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                name: "syncServiceSwitch"
                parentName: "deepinid/userinfo/body/syncService"
                displayName: qsTr("%1 Auto Sync").arg(dccData.editionName())
                description: qsTr("Securely store system settings and personal data in the cloud, and keep them in sync across devices")
                weight: 10
                pageType: DccObject.Editor
                page: Switch {

                }
                visible: false
            }
        }
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
