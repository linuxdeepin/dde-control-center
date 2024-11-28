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
        name: "checkUpdate"
        parentName: "update"
        backgroundType: DccObject.AutoBg
        displayName: qsTr("check update")
        description: qsTr("您有新的系统更新，请查看并更新")
        pageType: DccObject.Menu
        weight: 10
        visible: true
        page: DccGroupView {
            isGroup: false
            // height: implicitHeight
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: 60
                rightMargin: 60
            }

            CheckUpdate {
                id: checkUpdate
                visible: !dccData.model().showUpdateCtl
            }

            Component.onCompleted: {
                console.log(" checkUpdate : ", dccData.model().upgradable)
                dccData.work().checkUpgrade();
            }

        }

        UpdateControl {
        }

    }

    DccObject {
        visible: false

        name: "updateSettings"
        parentName: "update"
        displayName: qsTr("Update Settings")
        description: qsTr("You can set system updates, security updates, idle updates, update reminders, etc.")
        icon: "update"
        weight: 40
    }

}
