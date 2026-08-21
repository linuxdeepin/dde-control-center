// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    icon: "dialog-warning"
    width: 400
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    property string userName: "default"

    ColumnLayout {
        width: dialog.width - 20
        spacing: 30
        Label {
            id: label
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            text: qsTr("\"Auto Login\" can be enabled for only one account, please disable it for the account \"%1\" first").arg(userName)
        }

        Button {
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.fillWidth: true
            text: qsTr("Ok")
            onClicked: {
                close()
            }
        }
    }
}
