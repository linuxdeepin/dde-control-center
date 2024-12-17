// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    icon: "preferences-system"
    width: 600
    height: 180
    minimumWidth: 600
    minimumHeight: 180
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    property string msg

    signal requestShowSafePage()
    ColumnLayout {
        Label {
            Layout.alignment: Qt.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font: D.DTK.fontManager.t5
            text: msg
            wrapMode: Text.WordWrap
            Layout.preferredWidth: dialog.width - 10
            Layout.preferredHeight: 60
            leftPadding: 10
            rightPadding: 10
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.rightMargin: 10
            Layout.leftMargin: 10
            Layout.topMargin: 10
            Layout.fillWidth: true
            Button {
                text: qsTr("Go to settings")
                Layout.fillWidth: true
                onClicked: {
                    requestShowSafePage()
                    close()
                }
            }
            D.WarningButton {
                text: qsTr("Cancel")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    close()
                }
            }
        }
    }
}
