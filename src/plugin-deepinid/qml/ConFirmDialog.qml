// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    icon: "dcc_union_id"
    width: 420
    modality: Qt.WindowModal

    property string title
    property string message
    property string leftBtnText
    property string rightBtnText

    signal accepted()

    // FIXME: Label文本如果设置wrapMode换行，那么这里如果不指定dialog窗口的高，显示就有问题;并且这里要考虑中文不换行和英文会换行的情况，所以通过advance属性动态调整高度
    Component.onCompleted: {
        dialog.height = 175 + titleLable.advance.height + messageLable.advance.height
    }

    ColumnLayout {
        width: parent.width
        Label {
            id: titleLable
            Layout.fillWidth: true
            font: D.DTK.fontManager.t5
            text: dialog.title
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }

        Label {
            id: messageLable
            Layout.fillWidth: true
            font: D.DTK.fontManager.t7
            text: dialog.message
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }

        Item {
            Layout.fillWidth: true
            height: 20
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: 10
            spacing: 10
            Button {
                text: dialog.leftBtnText
                Layout.fillWidth: true
                onClicked: {
                    close()
                }
            }
            D.WarningButton {
                text: dialog.rightBtnText
                Layout.fillWidth: true
                onClicked: {
                    dialog.accepted()
                }
            }
        }
    }
}
