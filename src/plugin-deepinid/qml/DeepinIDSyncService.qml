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
    name: "syncService"
    parentName: "deepinid/userinfo/body"
    weight: 30
    pageType: DccObject.Item
    page: DccGroupView {}

    DccObject {
        name: "syncServiceSwitch"
        parentName: "deepinid/userinfo/body/syncService"
        displayName: qsTr("Auto Sync")
        description: qsTr("Securely store system settings and personal data in the cloud, and keep them in sync across devices")
        weight: 10
        pageType: DccObject.Editor
        page: Control {
            implicitWidth: rowlayout.implicitWidth
            implicitHeight: rowlayout.implicitHeight
            RowLayout {
                id: rowlayout
                spacing: 10
                Image {
                    id: warnImg
                    source: "qrc:/icons/deepin/builtin/icons/dcc_not_use_28px.svg"
                    visible: !dccData.model.syncEnabled
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            toolTip.visible = true
                        }
                        onExited: {
                            toolTip.visible = false
                        }
                    }
                }
                Switch {
                    checked: dccData.model.syncSwitch
                    enabled: dccData.model.syncEnabled
                    onCheckedChanged: {
                        dccData.worker.setAutoSync(checked)
                    }
                }
            }

            D.ToolTip {
                id: toolTip
                text: dccData.model.warnTipsMessage()
                y: warnImg.implicitHeight + 4
                visible: false
                delay: 0
            }
        }
    }

    DccObject {
        name: "systemSettings"
        parentName: "deepinid/userinfo/body/syncService"
        displayName: qsTr("System Settings")
        icon: "dcc_cfg_set"
        weight: 20
        backgroundType: DccObject.ClickStyle
        pageType: DccObject.Editor
        visible: dccData.model.syncSwitch
        page: D.IconLabel {
            Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            icon.name: dccData.model.syncItemShow ? "arrow_ordinary_up" : "arrow_ordinary_down"
        }
        onActive: {
            dccData.model.syncItemShow = !dccData.model.syncItemShow
        }
    }

    DccRepeater {
        model: dccData.model.syncInfoListModel()
        delegate: DccObject {
            name: model.type
            parentName: "deepinid/userinfo/body/syncService"
            weight: 30 + index
            icon: model.displayIcon
            displayName: model.displayName
            backgroundType: DccObject.ClickStyle
            visible: dccData.model.syncSwitch && dccData.model.syncItemShow
            pageType: DccObject.Editor
            property real iconSize: 16
            property real leftPaddingSize: 14
            page: DccCheckIcon {
                checked: model.isChecked
                onClicked: {
                    dccData.worker.setSyncSwitcher(model.keyList, !model.isChecked)
                }
            }
            onActive: {
                dccData.worker.setSyncSwitcher(model.keyList, !model.isChecked)
            }
        }
    }

    DccRepeater {
        model: dccData.model.appInfoListModel()
        delegate: DccObject {
            name: model.key
            parentName: "deepinid/userinfo/body/syncService"
            weight: 50 + index
            icon: model.icon
            displayName: model.name
            backgroundType: DccObject.ClickStyle
            visible: dccData.model.syncSwitch
            pageType: DccObject.Editor
            page: DccCheckIcon {
                checked: model.enable
                onClicked: {
                    dccData.worker.setUtcloudSwitcher(model.key, !model.enable)
                }
            }
            onActive: {
                dccData.worker.setUtcloudSwitcher(model.key, !model.enable)
            }
        }
    }

    DccObject {
        name: "syncTimeUpdate"
        parentName: "deepinid/userinfo/body/syncService"
        displayName: qsTr("Last sync time: %1").arg(dccData.model.lastSyncTime)
        weight: 100
        visible: dccData.model.syncSwitch
        pageType: DccObject.Editor
        page: D.ToolButton {
            text: qsTr("Clear cloud data")
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
                clearData.show()
            }

            ConfirmManager {
                id: clearData
                title: qsTr("Are you sure you want to clear your system settings and personal data saved in the cloud?")
                message: qsTr("Once the data is cleared, it cannot be recovered!")
                leftBtnText: qsTr("Cancel")
                rightBtnText: qsTr("Clear")

                onAccepted: {
                    dccData.worker.clearData()
                }
            }
        }
    }
}
