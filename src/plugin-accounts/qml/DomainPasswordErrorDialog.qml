// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    property string remoteTitle: ""
    property string remoteContent: ""
    property bool remoteRichText: false
    width: 400
    minimumWidth: width
    maximumWidth: minimumWidth
    leftPadding: 6
    rightPadding: 6
    icon: "preferences-system"
    modality: Qt.WindowModal

    ColumnLayout {
        width: parent.width
        Label {
            horizontalAlignment: Text.AlignHCenter
            font {
                family: D.DTK.fontManager.t6.family
                pixelSize: D.DTK.fontManager.t6.pixelSize
                bold: true
            }
            text: dialog.remoteTitle
            textFormat: Text.PlainText
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.topMargin: 10
        }

        Label {
            horizontalAlignment: Text.AlignHCenter
            font {
                family: D.DTK.fontManager.t8.family
                pixelSize: D.DTK.fontManager.t8.pixelSize
            }
            text: dialog.remoteContent
            wrapMode: Text.WordWrap
            textFormat: dialog.remoteRichText ? Text.RichText : Text.PlainText
            Layout.fillWidth: true
            Layout.topMargin: 7
        }

        Button {
            Layout.fillWidth: true
            Layout.topMargin: 33
            Layout.bottomMargin: 6

            text: qsTr("Confirm")
            onClicked: {
                dialog.close()
            }
        }
    }
}
