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
    title: qsTr("Enroll Finger")

    D.ListView {
        id: listview
        implicitWidth: dialog.width - DS.Style.dialogWindow.contentHMargin * 2
        implicitHeight: 500 - DS.Style.dialogWindow.titleBarHeight - 10
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
            ColumnLayout {
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: dialog.title
                    font.bold: true
                }

                Item {
                    Layout.preferredHeight: 50
                }

                D.DciIcon {
                    name: "user_biometric_fingerprint_add"
                    Layout.alignment: Qt.AlignCenter
                }

                Item {
                    Layout.fillHeight: true
                }

                Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    font: D.DTK.fontManager.t8
                    wrapMode: Text.WordWrap
                    text: qsTr("Place the finger to be entered into the fingerprint sensor and move it from bottom to top. After completing the action, please lift your finger.")
                    horizontalAlignment: Text.AlignHCenter
                }

                Item {
                    Layout.fillHeight: true
                }

                Row {
                    Layout.alignment: Qt.AlignCenter

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
                            listview.currentIndex = 2
                        }
                    }
                }

                D.RecommandButton {
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: 0
                    Layout.leftMargin: 6
                    Layout.rightMargin: 6
                    Layout.fillWidth: true
                    text: qsTr("Next")
                    enabled: agreeCheckbox.checked
                    onClicked: {
                        dccData.requestStartFingerEnroll();
                    }
                }
            }

            ColumnLayout {
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth

                Label {
                    text: dialog.title
                    font.bold: true
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                }

                Item {
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 200

                    D.DciIcon {
                        id: loaderIcon
                        anchors.centerIn: parent
                        name: dccData.fingertipImagePath
                        retainWhileLoading: true
                        sourceSize: Qt.size(100, 100)
                    }
                }

                Label {
                    Layout.fillWidth: true
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    text: dccData.fingerTitleTip
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    text: dccData.fingerMsgTip
                }


                Item {
                    Layout.minimumHeight: 60
                    Layout.fillHeight: true
                }

                RowLayout {
                    id: successBtnLayout
                    visible: dccData.addStage === CharaMangerModel.Success
                    spacing: 10
                    Layout.bottomMargin: 5
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Done")
                        onClicked: {
                            dialog.close()
                        }
                    }
                }

                RowLayout {
                    id: failedBtnLayout
                    visible: dccData.addStage === CharaMangerModel.Fail
                    spacing: 10
                    Layout.bottomMargin: 5
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Cancel")
                        onClicked: {
                            dccData.requestStopFingerEnroll()
                            dialog.close()
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

                RowLayout {
                    id: processBtnLayout
                    visible: dccData.addStage === CharaMangerModel.Processing
                    spacing: 10
                    Layout.bottomMargin: 5
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Cancel")
                        onClicked: {
                            dccData.requestStopFingerEnroll()
                            dialog.close()
                        }
                    }
                }
            }

            ColumnLayout {
                height: ListView.view.implicitHeight
                width: ListView.view.implicitWidth
                spacing: 5

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("Disclaimer")
                    font.bold: true
                }

                ScrollView {
                    id: scrollView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    D.Label {
                        width: scrollView.availableWidth
                        wrapMode: Text.WordWrap
                        text: qsTr(`"Biometric authentication" is a function for user identity authentication provided by UnionTech Software Technology Co., Ltd. Through "biometric authentication", the biometric data collected will be compared with that stored in the device, and the user identity will be verified based on the comparison result.

Please be noted that UnionTech Software Technology Co., Ltd. will not collect or access your biometric information, which will be stored on your local device. Please only enable the biometric authentication in your personal device and use your own biometric information for related operations, and promptly disable or delete other people's biometric information on that device, otherwise you will bear the risk arising therefrom.

UnionTech Software Technology Co., Ltd. is committed to research and improve the security, accuracy and stability of biometric authentication. However, due to environmental, equipment, technical and other factors and risk control, there is no guarantee that you will pass the biometric authentication temporarily. Therefore, please do not take biometric authentication as the only way to log in to UOS. If you have any questions or suggestions when using the biometric authentication, you can give feedback through "Service and Support" in the UOS.`)
                    }
                }

                RowLayout {
                    spacing: 10
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Cancel")
                        onClicked: {
                            listview.currentIndex = 0
                            agreeCheckbox.checked = false
                        }
                    }
                    D.RecommandButton {
                        Layout.fillWidth: true
                        text: qsTr("Agree")
                        onClicked: {
                            listview.currentIndex = 0
                            agreeCheckbox.checked = true
                        }
                    }
                }
            }
        }
        Connections {
            target: dccData.model
            function onEnrollResult(res) {
                switch(res) {
                    case CharaMangerModel.Enroll_Success:
                        listview.currentIndex = 1
                }
            }
        }
    }
}
