// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: root
    property real screenX: 0
    property real screenY: 0
    property real screenWidth: 1920
    property real screenHeight: 1080

    property string message: qsTr("Settings will be reverted in %1s.")
    property real timeout: 15
    property bool save: false
    modality: Qt.ApplicationModal
    width: 380
    x: screenX + ((screenWidth - width) / 2)
    y: screenY + ((screenHeight - height) / 2)
    icon: "preferences-system"
    title: qsTr("Save the display settings?")
    onClosing: {
        destroy(10)
        if (save) {
            dccData.saveChanges()
        } else {
            dccData.resetBackup()
        }
    }
    ColumnLayout {
        Timer {
            interval: 1000
            running: root.visible
            repeat: true
            onTriggered: {
                root.timeout--
                if (root.timeout < 1) {
                    close()
                }
            }
        }
        width: parent.width
        Label {
            Layout.fillWidth: true
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            text: title
            font.bold: true
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
        Label {
            Layout.fillWidth: true
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            text: message.arg(timeout)
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
        RowLayout {
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            D.RecommandButton {
                Layout.fillWidth: true
                text: qsTr("Revert")
                onClicked: close()
            }
            Rectangle {
                implicitWidth: 2
                Layout.fillHeight: true
                color: this.palette.button
            }

            D.Button {
                Layout.fillWidth: true
                text: qsTr("Save")
                onClicked: {
                    save = true
                    close()
                }
            }
        }
    }
}
