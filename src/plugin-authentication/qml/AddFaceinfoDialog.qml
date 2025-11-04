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
    title: qsTr("Enroll Face")

    onVisibleChanged: function() {
        if (listview.currentIndex != 0 && !visible) {
            dccData.faceController.stopEnroll()
        }
    }

    D.ListView {
        id: listview
        implicitWidth: dialog.width - dialog.leftPadding - dialog.rightPadding
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
                height: listview.implicitHeight
                width: listview.implicitWidth

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: dialog.title
                    font: D.DTK.fontManager.t5
                }

                Item {
                    Layout.preferredHeight: 20
                }

                D.DciIcon {
                    name: "user_biometric_face_add"
                    Layout.alignment: Qt.AlignCenter
                }

                Item {
                    Layout.fillHeight: true
                }

                ScrollView {
                    id: infoScrollView
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.preferredHeight: Math.min(infoLabel.implicitHeight, 120)
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded
                    
                    Component.onCompleted: {
                        if (contentItem) {
                            contentItem.interactive = true
                            contentItem.flickableDirection = Flickable.VerticalFlick
                            contentItem.boundsBehavior = Flickable.DragAndOvershootBounds
                        }
                    }
                    
                    Label {
                        id: infoLabel
                        width: infoScrollView.availableWidth
                        font: D.DTK.fontManager.t8
                        wrapMode: Text.WordWrap
                        text: qsTr("Face recognition does not support liveness detection, and the verification method may carry risks.\n\
To ensure successful entry:\n\
1. Keep your facial features clearly visible and do not cover them (hats, sunglasses, masks, etc.).\n\
2. Ensure sufficient lighting and avoid direct sunlight.")
                        horizontalAlignment: Text.AlignLeft
                        color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                Column {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                    spacing: 0

                    // 计算是否需要换行显示
                    property bool needWrap: {
                        // 估算文本宽度，如果总宽度超过可用空间则换行
                        var checkboxWidth = agreeCheckbox.implicitWidth
                        var buttonWidth = disclaimerButton.implicitWidth
                        var availableWidth = listview.implicitWidth - 40 // 减去边距
                        return (checkboxWidth + buttonWidth) > availableWidth
                    }

                    Row {
                        visible: !parent.needWrap
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 0

                        CheckBox {
                            id: agreeCheckbox
                            text: qsTr("I have read and agree to the")
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        D.ToolButton {
                            id: disclaimerButton
                            text: qsTr("Disclaimer")
                            padding: 0
                            background: null
                            font: agreeCheckbox.font
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.baseline: agreeCheckbox.baseline
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

                    // 换行显示时的布局
                    Column {
                        visible: parent.needWrap
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 0

                        CheckBox {
                            id: agreeCheckboxWrapped
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("I have read and agree to the")
                            checked: agreeCheckbox.checked
                            onCheckedChanged: agreeCheckbox.checked = checked
                            // 移除组件默认的边距
                            topPadding: 0
                            bottomPadding: 0
                        }

                        D.ToolButton {
                            id: disclaimerButtonWrapped
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("Disclaimer")
                            padding: 0
                            topPadding: 0
                            bottomPadding: 0
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

                    // 统一的checked状态管理
                    Connections {
                        target: agreeCheckboxWrapped
                        function onCheckedChanged() {
                            agreeCheckbox.checked = agreeCheckboxWrapped.checked
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
                    enabled: agreeCheckbox.checked || agreeCheckboxWrapped.checked
                    onClicked: {
                        dccData.faceController.startEnroll();
                        dialog.hide()
                    }
                }
            }

            // 添加中
            ColumnLayout {
                height: listview.implicitHeight
                width: listview.implicitWidth
                spacing: 0

                Label {
                    text: dialog.title
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    font: D.DTK.fontManager.t5
                }

                Item {
                    Layout.preferredHeight: 40
                }

                Item {
                    id: facePreviewItem
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: 250
                    Layout.preferredHeight: 250

                    D.DciIcon {
                        id: faceImg
                        visible: dccData.faceController.addStage === CharaMangerModel.Processing 
                        anchors.centerIn: parent
                        name: dccData.faceController.faceImgContent
                        sourceSize: Qt.size(210, 210)
                    }

                    Control {
                        id: loaderControl
                        visible: dccData.faceController.addStage === CharaMangerModel.Processing 
                        contentItem: D.DciIcon {
                            id: shortProgressCircle
                            anchors.fill: parent
                            sourceSize: Qt.size(250, 250)
                            name: "scan_loader"
                            palette: D.DTK.makeIconPalette(loaderControl.palette)
                        }
                    }

                    Timer {
                        repeat: true
                        running: true
                        interval: 50
                        onTriggered: {
                            loaderControl.rotation = loaderControl.rotation + 360 / 49
                        }
                    }
                }

                Item {
                    visible: dccData.faceController.addStage === CharaMangerModel.Processing 
                    Layout.preferredHeight: 50
                }

                Label {
                    text: dccData.faceController.enrollFaceTips
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
                height: listview.implicitHeight
                width: listview.implicitWidth

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: dialog.title
                    font: D.DTK.fontManager.t5
                }

                Item {
                    Layout.preferredHeight: 50
                }

                D.DciIcon {
                    name: dccData.faceController.enrollFaceSuccess ? "user_biometric_face_success" : "user_biometric_face_lose";
                    Layout.alignment: Qt.AlignCenter
                    sourceSize: Qt.size(150, 150)
                }

                Item {
                    Layout.preferredHeight: 30
                }

                Label {
                    visible: dccData.faceController.addStage === CharaMangerModel.Success || dccData.faceController.addStage === CharaMangerModel.Fail
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
                    text: dccData.faceController.addStage === CharaMangerModel.Success ? qsTr("Face enrolled") : qsTr("Failed to enroll your face")
                }

                Label {
                    text: dccData.faceController.enrollFaceTips
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
                    visible: dccData.faceController.addStage === CharaMangerModel.Success
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: 0
                    Layout.leftMargin: 0
                    Layout.rightMargin: 0

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Done")
                        onClicked: {
                            dccData.faceController.stopEnroll()
                            dialog.close()
                        }
                    }
                }

                RowLayout {
                    id: failedBtnLayout
                    visible: dccData.faceController.addStage === CharaMangerModel.Fail
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
                height: listview.implicitHeight
                width: listview.implicitWidth
                content: qsTr(`"Biometric authentication" is a function for user identity authentication provided by UnionTech Software Technology Co., Ltd. Through "biometric authentication", the biometric data collected will be compared with that stored in the device, and the user identity will be verified based on the comparison result.

Please be noted that UnionTech Software Technology Co., Ltd. will not collect or access your biometric information, which will be stored on your local device. Please only enable the biometric authentication in your personal device and use your own biometric information for related operations, and promptly disable or delete other people's biometric information on that device, otherwise you will bear the risk arising therefrom.

UnionTech Software Technology Co., Ltd. is committed to research and improve the security, accuracy and stability of biometric authentication. However, due to environmental, equipment, technical and other factors and risk control, there is no guarantee that you will pass the biometric authentication temporarily. Therefore, please do not take biometric authentication as the only way to log in to UOS. If you have any questions or suggestions when using the biometric authentication, you can give feedback through "Service and Support" in the UOS.`)
                onCancelClicked: {
                    listview.currentIndex = 0
                    agreeCheckbox.checked = false
                    agreeCheckboxWrapped.checked = false
                }
                onAgreeClicked: {
                    listview.currentIndex = 0
                    agreeCheckbox.checked = true
                    agreeCheckboxWrapped.checked = true
                }
            }
        }
        Connections {
            target: dccData.faceController

            function onAddStageChanged() {
                if (dccData.faceController.addStage === CharaMangerModel.Success) {
                    listview.currentIndex = 2
                } else if (dccData.faceController.addStage === CharaMangerModel.Fail) {
                    listview.currentIndex = 2
                }
            }
        }

        Connections {
            target: dccData.model
            function onTryStartInputFace(res) {
                listview.currentIndex = 1
                dialog.show()
            }
        }
    }
}
