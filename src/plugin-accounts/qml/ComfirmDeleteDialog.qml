// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    icon: "preferences-system"
    width: 400
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    signal requestDelete(bool deleteHome)
    ColumnLayout {
        width: dialog.width - 20
        Label {
            Layout.alignment: Qt.AlignHCenter
            font: D.DTK.fontManager.t5
            text: qsTr("Are you sure you want to delete this account?")
        }
        CheckBox {
            id: deleteHomeCheckbox
            checked: true // default checked
            text: qsTr("Delete account directory")
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.topMargin: 10
            Layout.fillWidth: true
            Button {
                text: qsTr("Cancel")
                Layout.preferredWidth: 180
                onClicked: {
                    close()
                }
            }
            Item {
                Layout.fillWidth: true
            }
            D.WarningButton {
                text: qsTr("Delete")
                Layout.preferredWidth: 180
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    requestDelete(deleteHomeCheckbox.checked)
                    close()
                }
            }
        }
    }
}
