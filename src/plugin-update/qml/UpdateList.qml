// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0



Rectangle {
    id: root
    property alias model: repeater.model
    property bool backgroundVisible: true
    property bool showPlayBtn: false
    signal clicked(int index, bool checked)
    signal playbtnClicked(int index)

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true

    Flickable {
        id: flickable
        width: parent.width
        height: parent.height
        contentWidth: parent.width
        contentHeight: repeater.childrenRect.height
        clip: true

        ColumnLayout {
            id: layoutView
            clip: true
            width: parent.width
            spacing: 10
            Repeater {
                id: repeater
                delegate: D.ItemDelegate {
                    id: itemCtl
                    Layout.fillWidth: true
                    leftPadding: 10
                    rightPadding: 10
                    implicitHeight: 80
                    cascadeSelected: true
                    backgroundVisible: true
                    contentFlow: true
                    spacing: 0

                    // text: model.name
                    // hoverEnabled: true
                    content:
                        RowLayout {
                        spacing: 10

                        D.DciIcon {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            name: "qrc:/icons/deepin/builtin/icons/system_update.png"
                            width: 36
                            height: 36
                            Layout.preferredWidth: 40
                        }

                        ColumnLayout {
                            Layout.alignment: Qt.AlignRight

                            RowLayout {
                                Label {
                                    Layout.alignment: Qt.AlignLeft
                                    text: model.name
                                    font.pixelSize: 14
                                    width: 100
                                    Layout.fillWidth: true
                                }

                                DccCheckIcon {
                                    visible: false
                                    Layout.alignment: Qt.AlignRight
                                    checked: model.checked
                                    size: 18
                                }
                            }

                            D.Label {
                                Layout.alignment: Qt.AlignLeft
                                horizontalAlignment: Text.AlignLeft
                                Layout.fillWidth: true
                                font.pixelSize: 12
                                text: qsTr("本次补丁更新主要提升了桌面环境、常用应用的操作性能体验，并且修复部分系统已知问题，进一步提升了产品质量。")
                                wrapMode: Text.WordWrap
                            }
                            D.Label {
                                visible: false
                                Layout.alignment: Qt.AlignLeft
                                horizontalAlignment: Text.AlignLeft
                                font.pixelSize: 12
                                text: qsTr("Updates：")
                            }

                            D.Label {
                                visible: false
                                Layout.alignment: Qt.AlignLeft
                                horizontalAlignment: Text.AlignLeft
                                font.pixelSize: 12
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            RowLayout {
                                visible: false
                                D.Label {
                                    Layout.alignment: Qt.AlignLeft
                                    horizontalAlignment: Text.AlignLeft
                                    font.pixelSize: 12
                                    text: qsTr("Release time：")
                                    Layout.fillWidth: true
                                }

                                D.ToolButton {
                                    flat: false
                                    Layout.alignment: Qt.AlignRight
                                    text: qsTr("View Details")
                                    highlighted: true
                                }
                            }
                        }
                    }


                    background: DccItemBackground {
                        separatorVisible: true
                        //highlightEnable: false
                    }

                    onClicked: {
                        root.clicked(index, !model.checked)
                    }
                }
            }
        }
    }

}

