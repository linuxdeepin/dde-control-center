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
    parentName: "deepinid/userinfo"
    weight: 30
    pageType: DccObject.Item
    page: DccGroupView {}

    DccObject {
        name: "syncServiceSwitch"
        parentName: "deepinid/userinfo/syncService"
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
        parentName: "deepinid/userinfo/syncService"
        displayName: qsTr("System Settings")
        icon: "dcc-systemcset"
        weight: 20
        backgroundType: DccObject.ClickStyle
        pageType: DccObject.Editor
        visible: dccData.model.syncSwitch

        page: Control {
            id: control
            rotation: dccData.model.syncItemShow ? 180 : 0
            Behavior on rotation {
                NumberAnimation {
                    duration: 200
                }
            }
            contentItem: D.DciIcon {
                name: "arrow_ordinary_down"
                sourceSize: Qt.size(12, 12)
                theme: D.DTK.themeType
                palette: D.DTK.makeIconPalette(control.palette)
            }
        }

        onActive: {
            dccData.model.syncItemShow = !dccData.model.syncItemShow
        }
    }

    DccRepeater {
        model: dccData.model.syncInfoListModel()
        delegate: DccObject {
            name: model.type
            parentName: "deepinid/userinfo/syncService"
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
            parentName: "deepinid/userinfo/syncService"
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
        parentName: "deepinid/userinfo/syncService"
        displayName: qsTr("Last sync time: %1").arg(dccData.model.lastSyncTime)
        weight: 100
        visible: dccData.model.syncSwitch
        pageType: DccObject.Item
        page: RowLayout {
            DccLabel {
                Layout.leftMargin: 10
                horizontalAlignment: Text.AlignLeft
                text: dccObj.displayName
                font: D.DTK.fontManager.t10
                opacity: 0.7
            }

            Item {
                Layout.fillWidth: true
            }

            D.ToolButton {
                Layout.preferredHeight: 40
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
}
