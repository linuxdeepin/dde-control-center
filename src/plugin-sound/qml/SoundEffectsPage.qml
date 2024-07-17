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
    readonly property int itemDelegateMinWidth: 204
    readonly property int itemDelegateMaxWidth: parent.width
    readonly property int itemDelegateHeight: 20
    anchors {
        left: parent.left
        top: parent.top
        topMargin: 10
    }
    id: control
    color: "white"
    //  height: parent.height
    height: dccData.model().getSoundEffectsRowCount() * 40 + 50
    //implicitHeight: dccData.model().getSoundEffectsRowCount() * 40 + 50
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
        ListView {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            implicitHeight: contentHeight
            width: parent.width - 40
            model: dccData.model().soundEffectsModel()
            visible: dccData.model().enableSoundEffect

            header: Rectangle {
                width: itemDelegateMaxWidth
                implicitWidth: itemDelegateMaxWidth
                height: 40
                color: "transparent"
                ColumnLayout{
                    width: parent.width
                    height: 40
                    RowLayout {
                        width: parent.width
                        implicitHeight: 40
                        Layout.preferredHeight: 40
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredWidth: parent.width

                        Text {
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("系统音效")
                            Layout.leftMargin: 10
                            font: DTK.fontManager.t4
                        }

                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: dccData.model().enableSoundEffect
                            onCheckedChanged: {
                                dccData.worker().enableAllSoundEffect(checked)
                            }
                            Layout.rightMargin: 10
                        }
                    }

                    Rectangle {
                        color: "#f2f2f2"
                        height: 1
                        Layout.preferredWidth: itemDelegateMaxWidth - 20
                        Layout.alignment: Qt.AlignHCenter
                    }

                }
            }

            delegate: ItemDelegate {
                width: itemDelegateMaxWidth
                height: 40
                backgroundVisible: false
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
                            font: DTK.fontManager.t4
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
                                    dccData.worker().setSoundEffectEnable(index, false)
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
                                onClicked: {
                                    dccData.worker().setSoundEffectEnable(index, true)
                                }
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
}
