// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window
import QtQml.Models
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

D.DialogWindow {
    id: dialog
    width: 360
    height: 500
    minimumWidth: width
    maximumWidth: minimumWidth
    modality: Qt.WindowModal
    title: qsTr("Enroll Face")

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: dialog.title
            font.bold: true
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
        }

        Item {
            id: facePreviewItem
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 250
            Layout.preferredHeight: 250

            D.DciIcon {
                id: faceImg
                anchors.centerIn: parent
                name: ""
                sourceSize: Qt.size(200, 200)
            }

            Image {
                id: shortProgressCircle
                width: 215
                height: 215
                anchors.centerIn: parent
                sourceSize: Qt.size(215, 215)
                source: "qrc:/accounts/icons/short_progress_circle.svg"
            }
        }

        Label {
            id: enrollResultLabel
            visible: false
            Layout.alignment: Qt.AlignCenter
            text: dccData.enrollSuccess ? qsTr("Face enrolled") : qsTr("Failed to enroll your face")
        }

        Text {
            id: tips
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            font: D.DTK.fontManager.t8
            wrapMode: Text.WordWrap
            text: dccData.enrollTips
            horizontalAlignment: Text.AlignHCenter
        }

        Item {
            Layout.minimumHeight: 60
            Layout.fillHeight: true
        }

        RowLayout {
            id: successBtnLayout
            visible: false
            spacing: 10
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.rightMargin: 20

            Button {
                Layout.fillWidth: true
                text: qsTr("Done")
                onClicked: {
                    close()
                }
            }
        }

        RowLayout {
            id: failedBtnLayout
            visible: false
            spacing: 10
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.rightMargin: 20

            Button {
                Layout.fillWidth: true
                text: qsTr("Cancel")
                onClicked: {
                    close()
                }
            }
            D.RecommandButton {
                Layout.fillWidth: true
                text: qsTr("Retry Enroll")
                onClicked: {
                    dccData.startFaceEnroll()
                    successBtnLayout.visible = false
                    failedBtnLayout.visible = false
                    enrollResultLabel.visible = false
                    shortProgressCircle.visible = true
                    faceImg.name = ""
                }
            }
        }

        Connections {
            target: dccData
            function onEnrollCompleted() {
                console.log("onEnrollCompleted called")
                faceImg.name = dccData.enrollSuccess ? "user_biometric_face_success": "user_biometric_face_lose"
                if (dccData.enrollSuccess) {
                    successBtnLayout.visible = true
                    failedBtnLayout.visible = false
                } else {
                    successBtnLayout.visible = false
                    failedBtnLayout.visible = true
                }

                enrollResultLabel.visible = true
                shortProgressCircle.visible = false
            }
            function onFaceImgContentChanged() {
                faceImg.name = dccData.faceImgContent
            }
        }
    }
}
