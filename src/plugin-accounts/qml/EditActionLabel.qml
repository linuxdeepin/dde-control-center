// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.LineEdit {
    id: edit
    property alias editBtn: editButton
    signal finished()

    readOnly: true
    background.visible: showAlert
    horizontalAlignment: TextInput.AlignLeft
    clearButton.visible: !readOnly
    rightPadding: clearButton.width + clearButton.anchors.rightMargin
    alertDuration: 3000
    onEditingFinished: {
        if (edit.readOnly)
            return
        if (showAlert)
            showAlert = false

        edit.readOnly = true

        finished()
    }

    D.ActionButton {
        id: editButton
        focusPolicy: Qt.NoFocus
        icon.name: "edit"
        icon.width: DS.Style.edit.actionIconSize
        icon.height: DS.Style.edit.actionIconSize
        background: null
        anchors {
            right: edit.right
            verticalCenter: edit.verticalCenter
        }
        onClicked: {
            edit.readOnly = false
            edit.selectAll()
            edit.forceActiveFocus()
        }
    }
}
