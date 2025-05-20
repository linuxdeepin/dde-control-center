// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

DccObject {
    id: root
    property var appList: dccData.appItemModels
    DccTitleObject {
        name: "doNotDisturbNotification"
        parentName: "notification"
        displayName: qsTr("Do Not Disturb Settings")
        weight: 10
    }
    DccObject {
        name: "enableDoNotDisturb"
        parentName: "notification"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "enableDoNotDisturbSwitch"
            parentName: "enableDoNotDisturb"
            description: qsTr("App notifications will not be shown on desktop and the sounds will be silenced, but you can view all messages in the notification center.")
            displayName: qsTr("Enable Do Not Disturb")
            weight: 10
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.sysItemModel.disturbMode
                onCheckedChanged: {
                    if (dccData.sysItemModel.disturbMode !== checked) {
                        dccData.sysItemModel.disturbMode = checked
                    }
                }
            }
        }
        DccObject {
            name: "enableDoNotDisturbTime"
            parentName: "enableDoNotDisturb"
            displayName: qsTr("Enable Do Not Disturb")
            icon: "notification"
            weight: 20
            pageType: DccObject.Item
            visible: dccData.sysItemModel.disturbMode
            page: TimeRange {
            }
        }
        DccObject {
            name: "enableDoNotDisturbLock"
            parentName: "enableDoNotDisturb"
            displayName: qsTr("Enable Do Not Disturb")
            icon: "notification"
            weight: 30
            pageType: DccObject.Item
            visible: dccData.sysItemModel.disturbMode
            page: RowLayout {
                D.CheckBox {
                    id: lockScreenCheckBox
                    implicitHeight: implicitContentHeight + 30
                    checked: dccData.sysItemModel.lockScreen
                    onCheckedChanged: {
                        if (dccData.sysItemModel.lockScreen !== checked) {
                            dccData.sysItemModel.lockScreen = checked
                        }
                    }
                }
                D.Label {
                    text: qsTr("When the screen is locked")
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: {
                            lockScreenCheckBox.checked = !lockScreenCheckBox.checked
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
    DccObject {
        name: "enableDoNotDisturb"
        parentName: "notification"
        displayName: qsTr("Number of notifications shown on the desktop")
        weight: 30
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.ComboBox {
            model: ["1", "2", "3"]
            flat: true
            currentIndex: dccData.sysItemModel.bubbleCount - 1
            onCurrentIndexChanged: {
                if (dccData.sysItemModel.bubbleCount - 1 !== currentIndex) {
                    dccData.sysItemModel.bubbleCount = currentIndex + 1
                }
            }
        }
    }

    DccTitleObject {
        name: "appNotify"
        parentName: "notification"
        displayName: qsTr("App Notifications")
        weight: 40
    }

    DccObject {
        id: applicationList
        name: "applicationList"
        parentName: "notification"
        weight: 50
        pageType: DccObject.Item
        page: DccGroupView {}
        DccRepeater {
            model: dccData.appListModel()
            delegate: DccObject {
                name: "notificationItem" + index
                parentName: "applicationList"
                pageType: DccObject.MenuEditor
                weight: 10 + index
                icon: model.AppIcon
                displayName: model.AppName
                backgroundType: DccObject.Normal
                page: D.Switch {
                    checked: model.EnableNotification
                    onCheckedChanged: {
                        if (model.EnableNotification !== checked) {
                            model.EnableNotification = checked
                        }
                    }
                }
                DccObject{
                    name: "notificationItemDetails" + index
                    parentName: "applicationList/" + "notificationItem" + index
                    DccObject{
                        backgroundType: DccObject.Normal
                        name: "allowNotifications" + index
                        parentName: "applicationList/notificationItem" + index + "/" + "notificationItemDetails" + index
                        displayName: qsTr("Allow Notifications")
                        description: qsTr("Display notification on desktop or show unread messages in the notification center")
                        icon: model.AppIcon
                        weight: 10
                        pageType: DccObject.Editor
                        page: D.Switch {
                            Layout.rightMargin: 10
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            checked: model.EnableNotification
                            onCheckedChanged: {
                                if (model.EnableNotification !== checked) {
                                    model.EnableNotification = checked
                                }
                            }
                        }
                    }
                    DccObject {
                        name: "notificationItemDetailsType" + index
                        parentName: "applicationList/notificationItem" + index + "/" + "notificationItemDetails" + index
                        backgroundType: DccObject.Normal
                        visible: model.EnableNotification
                        weight: 20
                        pageType: DccObject.Item
                        page: Rectangle {
                            color: "transparent"
                            implicitHeight: rowView.height + 20
                            RowLayout {
                                id: rowView
                                width: parent.width
                                anchors.centerIn: parent
                                ImageCheckBox {
                                    Layout.alignment: Qt.AlignCenter
                                    text: qsTr("Desktop")
                                    imageName: "notify_desktop"
                                    checked: model.ShowNotificationDesktop
                                    onCheckedChanged: {
                                        if (checked !== model.ShowNotificationDesktop) {
                                            model.ShowNotificationDesktop = checked
                                        }
                                    }
                                }
                                ImageCheckBox {
                                    Layout.alignment: Qt.AlignCenter
                                    text: qsTr("Lock Screen")
                                    imageName: "notify_lock"
                                    visible: false
                                    checked: model.LockScreenShowNotification
                                    onCheckedChanged: {
                                        if (checked !== model.LockScreenShowNotification) {
                                            model.LockScreenShowNotification = checked
                                        }
                                    }
                                }
                                ImageCheckBox {
                                    Layout.alignment: Qt.AlignCenter
                                    text: qsTr("Notification Center")
                                    imageName: "notify_center"
                                    checked: model.ShowNotificationCenter
                                    onCheckedChanged: {
                                        if (checked !== model.ShowNotificationCenter) {
                                            model.ShowNotificationCenter = checked
                                        }
                                    }
                                }
                            }
                        }
                    }
                    DccObject {
                        name: "notificationSettingsGroup" + index
                        parentName: "applicationList/notificationItem" + index + "/" + "notificationItemDetails" + index
                        pageType: DccObject.Item
                        backgroundType: DccObject.Normal
                        visible: model.EnableNotification
                        weight: 30
                        page: DccGroupView {}
                        DccObject {
                            name: "notificationPreview" + index
                            parentName: "applicationList/notificationItem" + index + "/" + "notificationItemDetails" + index +"/"+"notificationSettingsGroup" + index
                            displayName: qsTr("Show message preview")
                            pageType: DccObject.Item
                            weight: 10
                            page: D.CheckBox {
                                Layout.rightMargin: 10
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                text: dccObj.displayName
                                checked: model.EnablePreview
                                onCheckedChanged: {
                                    if (model.EnablePreview !== checked) {
                                        model.EnablePreview = checked
                                    }
                                }
                            }
                        }
                        DccObject {
                            name: "notificationSound" + index
                            parentName: "applicationList/notificationItem" + index + "/" + "notificationItemDetails" + index +"/"+"notificationSettingsGroup" + index
                            displayName: qsTr("Play a sound")
                            pageType: DccObject.Item
                            weight: 20
                            page: D.CheckBox {
                                Layout.rightMargin: 10
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                text: dccObj.displayName
                                checked: model.EnableSound
                                onCheckedChanged: {
                                    if (model.EnableSound !== checked) {
                                        model.EnableSound = checked
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
