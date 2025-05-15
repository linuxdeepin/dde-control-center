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
            dccData.stopFaceEnroll()
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
                    name: "user_biometric_face_add"
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
                    text: qsTr("Make sure all parts of your face are not covered by objects and are clearly visible. Your face should be well-lit as well.")
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
                        dccData.startFaceEnroll();
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
                    id: facePreviewItem
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: 250
                    Layout.preferredHeight: 250

                    D.DciIcon {
                        id: faceImg
                        visible: dccData.addStage === CharaMangerModel.Processing 
                        anchors.centerIn: parent
                        name: ""
                        sourceSize: Qt.size(210, 210)
                    }

                    Control {
                        id: loaderControl
                        visible: dccData.addStage === CharaMangerModel.Processing 
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
                    visible: dccData.addStage === CharaMangerModel.Processing 
                    Layout.preferredHeight: 50
                }

                Label {
                    text: dccData.enrollFaceTips
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
                    name: dccData.enrollFaceSuccess ? "user_biometric_face_success" : "user_biometric_face_lose";
                    Layout.alignment: Qt.AlignCenter
                    sourceSize: Qt.size(150, 150)
                }

                Item {
                    Layout.preferredHeight: 30
                }

                Label {
                    visible: dccData.addStage === CharaMangerModel.Success || dccData.addStage === CharaMangerModel.Fail
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
                    text: dccData.addStage === CharaMangerModel.Success ? qsTr("Face enrolled") : qsTr("Failed to enroll your face")
                }

                Label {
                    text: dccData.enrollFaceTips
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
                    visible: dccData.addStage === CharaMangerModel.Success
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: 0
                    Layout.leftMargin: 0
                    Layout.rightMargin: 0

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Done")
                        onClicked: {
                            dccData.stopFaceEnroll()
                            dialog.close()
                        }
                    }
                }

                RowLayout {
                    id: failedBtnLayout
                    visible: dccData.addStage === CharaMangerModel.Fail
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
                content: qsTr(`Before using face recognition, please note that: 
1. Your device may be unlocked by people or objects that look or appear similar to you.
2. Face recognition is less secure than digital passwords and mixed passwords.
3. The success rate of unlocking your device through face recognition will be reduced in a low-light, high-light, back-light, large angle scenario and other scenarios.
4. Please do not give your device to others randomly, so as to avoid malicious use of face recognition.
5. In addition to the above scenarios, you should pay attention to other situations that may affect the normal use of face recognition.

In order to better use of face recognition, please pay attention to the following matters when inputting the facial data:
1. Please stay in a well-lit setting, avoid direct sunlight and other people appearing in the recorded screen.
2. Please pay attention to the facial state when inputting data, and do not let your hats, hair, sunglasses, masks, heavy makeup and other factors to cover your facial features.
3. Please avoid tilting or lowering your head, closing your eyes or showing only one side of your face, and make sure your front face appears clearly and completely in the prompt box.`)
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
            target: dccData
            function onFaceImgContentChanged() {
                faceImg.name = dccData.faceImgContent;
            }

            function onAddStageChanged() {
                if (dccData.addStage === CharaMangerModel.Success) {
                    listview.currentIndex = 2
                } else if (dccData.addStage === CharaMangerModel.Fail) {
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
