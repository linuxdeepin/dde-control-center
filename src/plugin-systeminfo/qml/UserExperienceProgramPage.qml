// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

DccObject {
    name: "userExperienceProgramGrp"
    parentName: "system/userExperienceProgram"
    pageType: DccObject.Item
    weight: 20
    page: DccGroupView {}
    DccObject {
        name: "userExperienceProgramSwitch"
        weight: 20
        parentName: "system/userExperienceProgram/userExperienceProgramGrp"
        displayName: qsTr("Join User Experience Program")
        pageType: DccObject.Editor
        page: ColumnLayout {
            D.Switch {
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                checked: dccData.systemInfoMode().joinUeProgram
                onCheckedChanged: {
                    console.log("userExperienceProgramSwitch clicked ")
                    dccData.systemInfoWork().setUeProgram(checked)
                }
            }

            Window {
                id: modalDialog
                visible: false
                flags: Qt.Window
                modality: Qt.ApplicationModal
                color: "transparent"
                D.DWindow.enabled: true
                opacity: 0.0
            }

            Connections {
                target: dccData
                function onRequestUeProgram(visible) {

                    if (visible) {
                        modalDialog.show()
                    } else {
                        modalDialog.close()
                    }
                }
            }

        }
    }
    DccObject {
        name: "userExperienceProgramContent"
        weight: 30
        parentName: "system/userExperienceProgram/userExperienceProgramGrp"
        pageType: DccObject.Item
        page: D.Label {
            anchors.fill: parent
            anchors.margins: 10
            font: DTK.fontManager.t6
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
            opacity: 0.7
            text: dccData.systemInfoMode().userExperienceProgramText

            // 超链接点击事件
            onLinkActivated: function(url) {
                console.log("点击的链接是: " + url)
                Qt.openUrlExternally(url) // 使用默认浏览器打开链接
            }
        }
    }
}
