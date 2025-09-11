// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import QtQuick.Window
import QtQml.Models 2.1
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0
import org.deepin.dcc.account.biometric 1.0

D.DialogWindow {
    id: dialog
    width: 360
    height: 500
    minimumWidth: width
    maximumWidth: minimumWidth
    modality: Qt.WindowModal
    title: qsTr("Enroll Iris")

    onVisibleChanged: function() {
        if (listview.currentIndex != 0 && !visible) {
            dccData.irisController.stopEnroll()
        }
    }

    D.ListView {
        id: listview
        implicitWidth: dialog.width - DS.Style.dialogWindow.contentHMargin * 2
        implicitHeight: 500 - DS.Style.dialogWindow.titleBarHeight - DS.Style.dialogWindow.contentHMargin
        spacing: DS.Style.dialogWindow.contentHMargin
        model: itemModel
        orientation: ListView.Horizontal
        layoutDirection: Qt.LeftToRight
        snapMode: ListView.SnapOneItem
        boundsBehavior: Flickable.StopAtBounds
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightMoveDuration: 0
        interactive: false

        ObjectModel {
            id: itemModel
            // 添加前
            ColumnLayout {
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: dialog.title
                }

                Item {
                    Layout.preferredHeight: 50
                }

                D.DciIcon {
                    name: "iris_add"
                    Layout.alignment: Qt.AlignCenter
                }

                Item {
                    Layout.fillHeight: true
                }

                Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
                    font: D.DTK.fontManager.t8
                    wrapMode: Text.WordWrap
                    text: qsTr("Please keep an eye on the device and ensure that both eyes are within the collection area")
                    horizontalAlignment: Text.AlignHCenter
                    color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
                }

                Item {
                    Layout.fillHeight: true
                }

                Row {
                    Layout.alignment: Qt.AlignCenter
                    spacing: 0

                    CheckBox {
                        id: agreeCheckbox
                        text: qsTr("I have read and agree to the")
                    }

                    D.ToolButton {
                        text: qsTr("Disclaimer")
                        padding: 0
                        background: null
                        font: agreeCheckbox.font
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
                        onClicked: {
                            listview.currentIndex = 3
                        }
                    }
                }

                D.RecommandButton {
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: 0
                    Layout.leftMargin: 0
                    Layout.rightMargin: 0
                    Layout.fillWidth: true
                    text: qsTr("Next")
                    enabled: agreeCheckbox.checked
                    onClicked: {
                        dccData.irisController.startEnroll();
                        dialog.hide()
                    }
                }
            }

            // 添加中
            ColumnLayout {
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth
                spacing: 0

                Label {
                    text: dialog.title
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                }

                Item {
                    Layout.preferredHeight: 40
                }

                Item {
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: 250
                    Layout.preferredHeight: 250

                    D.DciIcon {
                        visible: dccData.irisController.addStage === CharaMangerModel.Processing 
                        anchors.centerIn: parent
                        name: "iris_scan"
                        sourceSize: Qt.size(250, 250)
                    }

                    Control {
                        id: loaderControl
                        visible: dccData.irisController.addStage === CharaMangerModel.Processing 
                        contentItem: D.DciIcon {
                            id: shortProgressCircle 
                            anchors.fill: parent
                            sourceSize: Qt.size(250, 250)
                            name: "iris_scanning"
                            palette: D.DTK.makeIconPalette(loaderControl.palette)
                        }
                    }   

                    // loaderControl 旋转动画
                    RotationAnimation {
                        target: loaderControl
                        property: "rotation"
                        from: 0
                        to: 360
                        duration: 2000
                        running: true
                        loops: Animation.Infinite
                    }
                }

                Item {
                    visible: dccData.irisController.addStage === CharaMangerModel.Processing 
                    Layout.preferredHeight: 50
                }

                Label {
                    text: dccData.irisController.enrollIrisTips
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: D.DTK.fontManager.t8
                    wrapMode: Text.WordWrap
                    color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
                }
                Item {
                    Layout.fillHeight: true
                }
            }

            // 完成页面
            ColumnLayout {
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: dialog.title
                }

                Item {
                    Layout.preferredHeight: 50
                }

                D.DciIcon {
                    name: dccData.irisController.enrollIrisSuccess ? "iris_success" : "iris_lose";
                    Layout.alignment: Qt.AlignCenter
                    sourceSize: Qt.size(150, 150)
                }

                Item {
                    Layout.preferredHeight: 30
                }

                Label {
                    visible: dccData.irisController.addStage === CharaMangerModel.Success || dccData.irisController.addStage === CharaMangerModel.Fail
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
                    text: dccData.irisController.addStage === CharaMangerModel.Success ? qsTr("Iris enrolled") : qsTr("Failed to enroll your iris")
                }

                Label {
                    text: dccData.irisController.enrollIrisTips
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: D.DTK.fontManager.t8
                    wrapMode: Text.WordWrap
                    color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
                }

                Item {
                    Layout.fillHeight: true
                }

                RowLayout {
                    id: successBtnLayout
                    visible: dccData.enrollIrisTips.addStage === CharaMangerModel.Success
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: 0
                    Layout.leftMargin: 0
                    Layout.rightMargin: 0

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Done")
                        onClicked: {
                            dccData.irisController.stopEnroll()
                            dialog.close()
                        }
                    }
                }

                RowLayout {
                    id: failedBtnLayout
                    visible: dccData.irisController.addStage === CharaMangerModel.Fail
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: 0
                    Layout.leftMargin: 0
                    Layout.rightMargin: 0

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Cancel")
                        onClicked: {
                            dialog.close();
                        }
                    }
                    D.RecommandButton {
                        Layout.fillWidth: true
                        text: qsTr("Retry Enroll")
                        onClicked: {
                            listview.currentIndex = 0
                        }
                    }
                }
            }
            // 免责声明
            DisclaimerControl {
                id: disclaimerControl
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth
                content: qsTr(`"Biometric authentication" is a function for user identity authentication provided by UnionTech Software Technology Co., Ltd. Through "biometric authentication", the biometric data collected will be compared with that stored in the device, and the user identity will be verified based on the comparison result.

Please be noted that UnionTech Software Technology Co., Ltd. will not collect or access your biometric information, which will be stored on your local device. Please only enable the biometric authentication in your personal device and use your own biometric information for related operations, and promptly disable or delete other people's biometric information on that device, otherwise you will bear the risk arising therefrom.

UnionTech Software Technology Co., Ltd. is committed to research and improve the security, accuracy and stability of biometric authentication. However, due to environmental, equipment, technical and other factors and risk control, there is no guarantee that you will pass the biometric authentication temporarily. Therefore, please do not take biometric authentication as the only way to log in to UOS. If you have any questions or suggestions when using the biometric authentication, you can give feedback through "Service and Support" in the UOS.`)
                onCancelClicked: {
                    listview.currentIndex = 0
                    agreeCheckbox.checked = false
                }
                onAgreeClicked: {
                    listview.currentIndex = 0
                    agreeCheckbox.checked = true
                }
            }
        }
        Connections {
            target: dccData.irisController

            function onAddStageChanged() {
                if (dccData.irisController.addStage === CharaMangerModel.Success) {
                    listview.currentIndex = 2
                } else if (dccData.irisController.addStage === CharaMangerModel.Fail) {
                    listview.currentIndex = 2
                }
            }
        }

        Connections {
            target: dccData.model
            function onTryStartInputIris(res) {
                listview.currentIndex = 1
                dialog.show()
            }
        }
    }
}
