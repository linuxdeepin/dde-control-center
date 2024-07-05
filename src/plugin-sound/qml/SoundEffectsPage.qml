// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0

import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    Layout.preferredWidth: parent.width
    Layout.preferredHeight: parent.height

    color: "#f3f6f8"
    border.color: "white"
    border.width: 1

    width: parent.width
    height: parent.height
    RowLayout {
        height: dccData.model().getSoundEffectsRowCount() * 40 + 80
        width: parent.width
        Layout.preferredWidth: parent.width

        Rectangle {

            readonly property int itemDelegateMinWidth: 204
            readonly property int itemDelegateMaxWidth: parent.width - 150
            readonly property int itemDelegateHeight: 20
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: 60 // 设置左边间距为20
                rightMargin: 60
                topMargin: 30
            }
            id: control
            color: "white"
            height: parent.height
            width: itemDelegateMaxWidth
            border.color: "white"
            border.width: 1
            radius: 10
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.preferredWidth: parent.width - 120
            Layout.topMargin: 10

            ColumnLayout {
                width: parent.width
                height: parent.height
                Layout.preferredHeight: parent.height
                ColumnLayout {
                    width: parent.width
                    height: 40
                    Layout.preferredHeight: 40
                    RowLayout {
                        width: parent.width
                        height: parent.height
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredWidth: parent.width

                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            text: qsTr("系统音效")
                            Layout.leftMargin: 10
                        }

                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: true
                        }
                    }

                    Rectangle {
                        color: "#f2f2f2"
                        height: 1
                        Layout.preferredWidth: control.width - 20
                    }
                }

                ListView {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    implicitHeight: contentHeight
                    width: parent.width - 40
                    model: dccData.model().soundEffectsModel()

                    delegate: ItemDelegate {
                        width: itemDelegateMaxWidth
                        height: 40
                        backgroundVisible: true
                        corners: getCornersForBackground(index,
                                                         ListView.view.count)
                        contentFlow: true
                        cascadeSelected: true
                        content: ColumnLayout {
                            width: control.width
                            Layout.preferredHeight: 40

                            RowLayout {
                                spacing: 5
                                width: control.width - 20
                                Layout.preferredHeight: 40
                                Layout.preferredWidth: control.width - 20

                                Label {
                                    text: model.name
                                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                }

                                RowLayout {
                                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                    IconButton {
                                        icon.name: "qrc:/icons/deepin/builtin/actions/checked.png"
                                        icon.width: 24
                                        icon.height: 24
                                        implicitWidth: 24
                                        background: Rectangle {
                                            color: "transparent" // 背景透明
                                            border.color: "transparent" // 边框透明
                                            border.width: 0
                                        }
                                        visible: model.isChecked
                                        onClicked: {
                                            dccData.worker().setPausePlayer(checked)
                                        }
                                    }
                                    IconButton {
                                        icon.name: "qrc:/icons/deepin/builtin/actions/nocheck.png"
                                        icon.width: 24
                                        icon.height: 24
                                        implicitWidth: 24
                                        background: Rectangle {
                                            color: "transparent" // 背景透明
                                            border.color: "transparent" // 边框透明
                                            border.width: 0
                                        }
                                        visible: !model.isChecked
                                    }
                                }
                            }

                            Rectangle {
                                color: "#f2f2f2"
                                height: 1
                                Layout.preferredWidth: control.width - 20
                            }
                        }
                    }
                }
            }
            // }
        }
    }
}
