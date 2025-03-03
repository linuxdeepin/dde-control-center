// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

ColumnLayout {
    id: rootLayout
    property alias updateListModels: updatelistModel.model;

    width: parent.width
    RowLayout {
        Layout.preferredWidth: parent.width
        ColumnLayout {
            Layout.leftMargin: 22
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            spacing: 5
            RowLayout {
                spacing: 5
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                DccCheckIcon {
                    visible: dccData.model().distUpgradeState === 3
                    checked: true
                    size: 18
                }

                D.Label {
                    font.pixelSize: 16
                    font.bold: true
                    text: dccData.model().updateStateTips
                }
            }

            RowLayout {
                spacing: 5
                visible: false
                D.Label {
                    text: qsTr("Expected installation time:")
                    font.pixelSize: 12
                }

                D.Label {
                    text: qsTr("30min")
                    font.pixelSize: 12
                }
            }
        }

        D.Button {
            Layout.rightMargin: 12
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: dccData.model().actionBtnText
            font.pixelSize: 14
            textColor: DS.Style.highlightedButton.background1

            visible: dccData.model().distUpgradeState !== 1 && dccData.model().distUpgradeState !== 0
            onClicked: {
                dccData.work().onActionBtnClicked();
            }
        }

        D.BusyIndicator {
            id: scanAnimation

            Layout.alignment: Qt.AlignRight
            running: dccData.model().distUpgradeState === 1 || dccData.model().distUpgradeState === 0
            visible: dccData.model().distUpgradeState === 1 || dccData.model().distUpgradeState === 0
            implicitWidth: 32
            implicitHeight: 32
        }

        ColumnLayout {
            visible: false
           // visible: dccData.model().distUpgradeState === 1
            Layout.rightMargin: 12
            Layout.alignment: Qt.AlignRight

            RowLayout {
                id: progressCtl
                spacing: 10
                Layout.alignment: Qt.AlignRight
                D.ProgressBar {
                    id: process
                    Layout.alignment: Qt.AlignHCenter
                    from: 0
                    to: 100
                    value: 45
                    implicitHeight: 8
                    implicitWidth: 240
                }

                D.DciIcon {
                    id: stratIcon
                    name: "qrc:/icons/deepin/builtin/icons/update_stop.png"
                    width: 16
                    height: 16
                }

                D.DciIcon {
                    id: stopIcon
                    name: "qrc:/icons/deepin/builtin/icons/update_close.png"
                    width: 16
                    height: 16
                }
            }

            Label {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: stratIcon.width + stopIcon.width + progressCtl.spacing * 2
                width: parent.width
                text: qsTr("Installing") + process.value + "%"
                font.pixelSize: 12
            }
        }
    }

    Rectangle {
        height: 1
        color: D.DTK.themeType === D.ApplicationHelper.LightType ?
            Qt.rgba(0, 0, 0, 0.05) : Qt.rgba(1, 1, 1, 0.05)
        Layout.leftMargin: 10
        Layout.rightMargin: 10
        Layout.fillWidth: true
    }

    UpdateList {
        id: updatelistModel

    }
}
