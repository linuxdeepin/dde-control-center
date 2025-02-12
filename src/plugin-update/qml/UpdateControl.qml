// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "updateCtl"
        parentName: "checkUpdate"
        backgroundType: DccObject.AutoBg
        weight: 10
        visible: dccData.model().showUpdateCtl
        pageType: DccObject.Item

        page: RowLayout {
            ColumnLayout {
                RowLayout {
                    spacing: 0
                    DccCheckIcon {
                        visible: dccData.model().updateState === "success"
                        checked: true
                        size: 18
                    }

                    D.Label {
                        Layout.leftMargin: 10
                        font.pixelSize: 18
                        font.bold: true
                        text: dccData.model().updateStateTips
                    }
                }

                D.Label {
                    visible: false
                    text: qsTr("预计安装时间：30min")
                    font.pixelSize: 12
                }
            }

            D.Button {
                id: actionBtn
                Layout.alignment: Qt.AlignRight
                text: dccData.model().actionBtnText
                font.pixelSize: 14
                implicitWidth: DS.Style.control.contentImplicitWidth(actionBtn) + 10
                visible: dccData.model().updateState !== "upgrading"
                onClicked: {
                    dccData.work().onActionBtnClicked();
                }
            }

            D.BusyIndicator {
                id: scanAnimation

                Layout.alignment: Qt.AlignRight
                running: dccData.model().updateState === "upgrading"
                visible: dccData.model().updateState === "upgrading"
                implicitWidth: 32
                implicitHeight: 32
            }

            ColumnLayout {
                visible: false
                Layout.alignment: Qt.AlignRight

                RowLayout {
                    spacing: 10
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
                        name: "qrc:/icons/deepin/builtin/icons/update_stop.png"
                        width: 16
                        height: 16
                    }

                    D.DciIcon {
                        name: "qrc:/icons/deepin/builtin/icons/update_close.png"
                        width: 16
                        height: 16
                    }
                }

                Label {
                    text: qsTr("Installing") + process.value + "%"
                    font.pixelSize: 12
                }
            }
        }
    }

    DccObject {
        name: "updateList"
        parentName: "checkUpdate"
        backgroundType: DccObject.Normal
        weight: 20
        visible: dccData.model().showUpdateCtl
        pageType: DccObject.Item

        page: UpdateList {
            model: ListModel {
                ListElement {
                    name: qsTr("Feature Updates")
                    checked: true
                }
            }
        }
    }
}
