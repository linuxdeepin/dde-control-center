// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

DccObject {
    AccountSettings {
        id: currentAccount
        name: "currentAccount"
        papaName: "accounts"
        userId: dccData.currentUserId()
    }

    // 其他账户
    DccObject {
        name: "otherAcountsTitle"
        parentName: "accounts"
        displayName: qsTr("Other accounts")
        weight: 58
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
    }

    DccObject {
        name: "otherAcounts"
        parentName: "accounts"
        weight: 60
        pageType: DccObject.Item
        page: DccGroupView {}

        DccRepeater {
            id: otherAccountsRepeater
            model: dccData.accountsModel()
            delegate: DccObject {
                name: "otherAccountsItem" + index
                parentName: "otherAcounts"
                description: model.userType
                icon: model.avatar
                weight: 10 + 10 * index
                pageType: DccObject.Item
                page: D.ItemDelegate {
                    id: menuItemDelegate
                    implicitHeight: 50
                    property alias separatorVisible: background.separatorVisible
                    property real iconRadius: 8
                    property real iconSize: 32

                    Layout.fillWidth: true
                    backgroundVisible: false
                    checkable: false
                    topPadding: topInset
                    bottomPadding: bottomInset
                    leftPadding: 10
                    rightPadding: 8
                    icon {
                        name: dccObj.icon
                        width: menuItemDelegate.iconSize
                        height: menuItemDelegate.iconSize
                    }
                    contentItem: RowLayout {
                        Item {
                            implicitWidth: icon.width + 10
                            implicitHeight: icon.height + 10
                            D.IconLabel {
                                id: iconLabel
                                anchors.centerIn: parent
                                spacing: menuItemDelegate.spacing
                                mirrored: menuItemDelegate.mirrored
                                display: menuItemDelegate.display
                                alignment: Qt.AlignLeft | Qt.AlignVCenter
                                icon: D.DTK.makeIcon(menuItemDelegate.icon,
                                                     menuItemDelegate.D.DciIcon)
                            }

                            D.BoxShadow {
                                hollow: true
                                anchors.fill: iconLabel
                                shadowBlur: 10
                                spread: 10
                                shadowColor: palette.base
                                cornerRadius: menuItemDelegate.iconRadius
                            }

                            Rectangle {
                                id: onlineIndicator
                                visible: model.online
                                width: 10
                                height: 10
                                radius: 6
                                z: 10
                                color: "#67EF4A"
                                border.color: "white"
                                anchors {
                                    right: parent.right
                                    bottom: parent.bottom
                                    rightMargin: 3
                                    bottomMargin: 3
                                }
                            }
                        }

                        Loader {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignRight
                            sourceComponent: RowLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                ColumnLayout {
                                    Layout.leftMargin: 8
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter
                                    spacing: 0
                                    DccLabel {
                                        Layout.fillWidth: true
                                        text: model.display
                                    }
                                    DccLabel {
                                        Layout.fillWidth: true
                                        visible: text !== ""
                                        font: D.DTK.fontManager.t8
                                        text: dccObj.description
                                        opacity: 0.5
                                    }
                                }
                                // rightItem
                                Control {
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    Layout.topMargin: 5
                                    Layout.bottomMargin: 5
                                    contentItem: D.IconLabel {
                                        icon.name: "arrow_ordinary_right"
                                    }
                                }
                            }
                        }
                    }

                    background: DccItemBackground {
                        id: background
                        separatorVisible: true
                    }
                    onClicked: {
                        otherSettings.displayName = model.display
                        otherSettings.userId = model.userId
                        DccApp.showPage(otherSettings)
                    }
                }

            }
        }

        DccObject {
            name: "otherSettingsHolder"
            parentName: "accounts"
            pageType: DccObject.Item
            page: Item { }

            AccountSettings {
                id: otherSettings
                name: "otherAccountSettings"
                parentName: "otherSettingsHolder"
                papaName: name
            }
        }
    }
}
