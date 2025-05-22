// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

Control {
    id: root
    property alias model: repeater.model
    property bool backgroundVisible: true
    signal clicked(int index, bool checked)

    enum WindowEffectType {
        Default = 0,
        Best,
        Better,
        Good,
        Normal,
        Compatible
    }

    Layout.fillWidth: true
    contentItem: ColumnLayout {
        id: layoutView
        width: parent.width
        spacing: 0
        Repeater {
            id: repeater
            model: listModel
            delegate: D.ItemDelegate {
                Layout.fillWidth: true
                leftPadding: 10
                rightPadding: 10
                cascadeSelected: true
                checkable: false
                contentFlow: true
                corners: getCornersForBackground(index, listModel.count)
                icon.name: model.icon
                content: RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ColumnLayout {
                        Layout.leftMargin: 5
                        Layout.fillWidth: true
                        spacing: 2
                        DccLabel {
                            Layout.fillWidth: true
                            font: D.DTK.fontManager.t6
                            text: model.title
                        }
                        DccLabel {
                            Layout.fillWidth: true
                            visible: text !== ""
                            font: D.DTK.fontManager.t10
                            text: model.description
                            opacity: 0.5
                        }
                    }
                    Control {
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        contentItem: DccCheckIcon {
                            visible: model.value === dccData.model.windowEffectType
                        }
                    }
                }
                background: DccItemBackground {
                    separatorVisible: true
                    backgroundType: DccObject.ClickStyle
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        dccData.worker.setWindowEffect(model.value)
                    }
                }
            }
        }
    }

    ListModel {
        id: listModel

        ListElement {
            value: InterfaceEffectListview.WindowEffectType.Normal
            title: qsTr("Optimal Performance")
            icon: "optimum_performance"
            description: qsTr("Disable all interface and window effects for efficient system performance.")
        }

        ListElement {
            value: InterfaceEffectListview.WindowEffectType.Better
            title: qsTr("Balance")
            icon: "balance"
            description: qsTr("Limit some window effects for excellent visuals while maintaining smooth system performance.")
        }

        ListElement {
            value: InterfaceEffectListview.WindowEffectType.Best
            title: qsTr("Best Visuals")
            icon: "best_vision"
            description: qsTr("Enable all interface and window effects for the best visual experience.")
        }
    }
}
