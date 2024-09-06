// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: repeater.model
    property bool backgroundVisible: true
    signal clicked(int index, bool checked)

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true

    ColumnLayout {
        id: layoutView
        width: parent.width
        clip: true
        spacing: 0
        Repeater {
            id: repeater
            model: powerModeModel
            delegate: D.ItemDelegate {
                Layout.fillWidth: true
                visible: {
                    if (model.mode === "performance") {
                        return dccData.model.isHighPerformanceSupported
                    } else if (model.mode === "balance_performance") {
                        return dccData.model.isBalancePerformanceSupported
                    }
                    return true
                }

                leftPadding: 10
                rightPadding: 10
                cascadeSelected: true
                checkable: false
                contentFlow: true
                corners: getCornersForBackground(index, powerModeModel.count)
                icon.name: model.icon
                content: RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ColumnLayout {
                        Layout.fillWidth: true
                        DccLabel {
                            Layout.fillWidth: true
                            text: model.title
                        }
                        DccLabel {
                            Layout.fillWidth: true
                            visible: text !== ""
                            font: D.DTK.fontManager.t8
                            text: model.description
                            opacity: 0.5
                        }
                    }
                    Control {
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        contentItem: D.IconButton {
                            icon.name: model.mode === dccData.model.powerPlan ? "qrc:/icons/deepin/builtin/actions/checked.png" : "qrc:/icons/deepin/builtin/actions/nocheck.png"
                            icon.width: 24
                            icon.height: 24
                            implicitWidth: 36
                            implicitHeight: 36
                            background: Rectangle {
                                color: "transparent"
                                border.color: "transparent"
                                border.width: 0
                            }
                        }
                    }
                }
                background: DccListViewBackground {
                    separatorVisible: true
                    highlightEnable: false
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        dccData.worker.setPowerPlan(model.mode)
                    }
                }
            }
        }
    }

    ListModel {
        id: powerModeModel

        ListElement {
            mode: "performance"
            title: qsTr("高性能模式")
            icon: "dcc_nav_systeminfo"
            description: qsTr("性能优先, 会显著提升功耗和发热")
        }

        ListElement {
            mode: "balance_performance"
            title: qsTr("性能模式")
            icon: "dcc_nav_systeminfo"
            description: qsTr("根据负载积极调整运行频率")
        }

        ListElement {
            mode: "balance"
            title: qsTr("平衡模式")
            icon: "dcc_nav_systeminfo"
            description: qsTr("兼顾性能和续航, 根据使用情况自动调节")
        }

        ListElement {
            mode: "powersave"
            title: qsTr("节能模式")
            icon: "dcc_nav_systeminfo"
            description: qsTr("续航优先, 系统会牺牲一些性能表现来降低功耗")
        }
    }
}
