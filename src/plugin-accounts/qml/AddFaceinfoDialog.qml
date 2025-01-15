// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
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
            height: 50
        }

        D.DciIcon {
            name: "user_biometric_face_add"
            Layout.alignment: Qt.AlignCenter
        }

        Item {
            Layout.fillHeight: true
        }

        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            font: D.DTK.fontManager.t8
            wrapMode: Text.WordWrap
            text: qsTr("Make sure all parts of your face are not covered by objects and are clearly visible. Your face should be well-lit as well.")
            horizontalAlignment: Text.AlignHCenter
        }

        Item {
            height: 80
        }

        Row {
            Layout.alignment: Qt.AlignCenter
            CheckBox {
                id: agreeCheckbox
            }
            Label {
                text: qsTr("I have read and agree to the")
            }
            Label {
                textFormat: Text.RichText
                text: qsTr('<a href="test">Disclaimer</a>')
                onLinkActivated: (link) => {
                    console.log("Todo: Show disclaimer dialog")
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
                dialog.hide()
                enrollFaceDialogLoader.active = true
            }
            Loader {
                id: enrollFaceDialogLoader
                active: false
                sourceComponent: EnrollFaceDialog {
                    onClosing: function
                    (close) {
                        console.log("on closing called")
                        enrollFaceDialogLoader.active = false
                        dccData.stopEnroll()
                    }
                }
                onLoaded: function () {
                    dccData.startFaceEnroll()
                    enrollFaceDialogLoader.item.show()
                }
            }
        }
    }
}
