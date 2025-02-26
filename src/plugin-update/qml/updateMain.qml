// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15

DccObject {
    DccObject {
        name: "noActive11"
        parentName: "update"
        pageType: DccObject.Item
        backgroundType: DccObject.Audobg
        weight: 10
        visible: false
        page: NoActive{}
    }

    DccObject {
        name: "checkUpdatePage"
        parentName: "update"
        //displayName: qsTr("check update")
        //description: dccData.model().upgradable ? qsTr("Your system is already the latest version") : qsTr("You have a new system update, please check and update")
        pageType: DccObject.Item
        backgroundType: DccObject.Audobg
        visible: !dccData.model().showUpdateCtl
        weight: 20

        page: CheckUpdate{}
    }

    DccObject {
        name: "installCompleteList"
        parentName: "update"
        backgroundType: DccObject.Normal
        weight: 30
        visible: dccData.model().showUpdateCtl
        pageType: DccObject.Item

        page: UpdateControl{

            updateListModels: ListModel {
                    ListElement {
                        name: qsTr("Feature Updates")
                        checked: true
                    }
                }
        }
    }


    DccObject {
        name: "installingCompleteList"
        parentName: "update"
        backgroundType: DccObject.Normal
        weight: 40
        visible: false
        //    visible: dccData.model().showUpdateCtl
        pageType: DccObject.Item
        page: UpdateControl{

            updateListModels: ListModel {
                ListElement {
                    name: qsTr("Feature Updates")
                    checked: true
                }
                ListElement {
                    name: qsTr("Feature Updates")
                    checked: true
                }
                ListElement {
                    name: qsTr("Feature Updates")
                    checked: true
                }
            }
        }
    }

    DccObject {
        name: "preInstallList"
        parentName: "update"
        backgroundType: DccObject.Normal
        weight: 50
        pageType: DccObject.Item
        visible: false
        page:  UpdateControl{

        }
    }


    DccObject {
        name: "downloadingList"
        parentName: "update"
        backgroundType: DccObject.Normal
        weight: 60
        pageType: DccObject.Item
        visible: false
        page:  UpdateControl{}
    }

    DccObject {
        name: "preUpdateList"
        parentName: "update"
        backgroundType: DccObject.Normal
        weight: 70
        pageType: DccObject.Item
        visible: false
        page: UpdateList {
            model: ListModel {
                ListElement {
                    name: qsTr("Feature Updates")
                    checked: true
                }
            }
        }
    }

    DccObject {
        name: "updateSettingsPage"
        parentName: "update"
        displayName: qsTr("Update Settings")
      //  description: qsTr("You can set system updates, security updates, idle updates, update reminders, etc.")
        icon: "update_set"
        weight: 100

        visible: DccApp.uosEdition() === DccApp.UosCommunity

        UpdateSetting{}
    }

}
