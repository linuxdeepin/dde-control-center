// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

ColumnLayout {  
    id: control
    property alias content: label.text

    signal cancelClicked()
    signal agreeClicked()

    spacing: 5
    Label {
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Disclaimer")
    }

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.fillHeight: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        contentWidth: scrollView.width

        D.Label {
            id: label
            width: scrollView.availableWidth
            wrapMode: Text.WordWrap
        }
    }

    RowLayout {
        spacing: 10
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        Layout.topMargin: 0
        Layout.leftMargin: 0
        Layout.rightMargin: 0

        Button {
            Layout.fillWidth: true
            text: qsTr("Cancel")
            onClicked: {
                control.cancelClicked()
            }
        }
        D.RecommandButton {
            Layout.fillWidth: true
            text: qsTr("Agree")
            onClicked: {
                control.agreeClicked()
            }
        }
    }
}