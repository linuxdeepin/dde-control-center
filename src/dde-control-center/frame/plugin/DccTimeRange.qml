// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.SpinBox {
    id: control
    readonly property string timeString: (hour < 10 ? "0" + Number(hour) : Number(hour)) + ":" + (minute < 10 ? "0" + Number(minute) : Number(minute))
    property int hour: 0
    property int minute: 0
    property var curInput: hourInput
    Layout.maximumWidth: 110
    from: 0
    to: 23
    value: 8
    editable: true
    font: D.DTK.fontManager.t7
    signal timeChanged

    function updateValue() {
        const isHourInput = (curInput === hourInput)
        const targetProperty = isHourInput ? hour : minute

        if (targetProperty !== value) {
            if (isHourInput) {
                hour = value
            } else {
                minute = value
            }
            control.timeChanged()
        }
    }

    contentItem: RowLayout {
        spacing: 0
        TextInput {
            id: hourInput
            Layout.preferredWidth: 20
            Layout.alignment: Qt.AlignHCenter
            text: control.hour < 10 ? "0" + Number(control.hour) : control.hour
            font: control.font
            color: control.palette.text
            selectionColor: control.palette.highlight
            selectedTextColor: control.palette.highlightedText
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            leftPadding: DS.Style.spinBox.spacing
            readOnly: !control.editable
            validator: control.validator
            maximumLength: 2
            inputMethodHints: control.inputMethodHints
            selectByMouse: true
            mouseSelectionMode: TextInput.SelectCharacters
            onFocusChanged: {
                if (focus) {
                    curInput = this
                }
                if (0 === text.length || 0 === Number(text)) {
                    control.hour = 0
                }
            }
            onTextChanged: {
                if (curInput === this) {
                    control.value = Number(text)
                }
            }
        }
        Label {
            Layout.preferredWidth: 10
            Layout.alignment: Qt.AlignHCenter
            text: ":"
            font: control.font
            color: control.palette.text
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            leftPadding: DS.Style.spinBox.spacing
        }
        TextInput {
            id: minuteInput
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 20
            text: control.minute < 10 ? "0" + Number(control.minute) : control.minute
            font: control.font
            color: control.palette.text
            selectionColor: control.palette.highlight
            selectedTextColor: control.palette.highlightedText
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            leftPadding: DS.Style.spinBox.spacing
            readOnly: !control.editable
            validator: control.validator
            maximumLength: 2
            inputMethodHints: control.inputMethodHints
            selectByMouse: true
            mouseSelectionMode: TextInput.SelectCharacters
            onFocusChanged: {
                if (focus) {
                    curInput = this
                }
                if (0 === text.length || 0 === Number(text)) {
                    control.minute = 0
                }
            }
            onTextChanged: {
                if (curInput === this) {
                    control.value = Number(text)
                }
            }
        }
    }
    onCurInputChanged: {
        if (curInput === hourInput) {
            to = 23
            value = hour
        } else {
            to = 59
            value = minute
        }
    }
    onValueChanged: {
        // onValueChanged early than onCurInputChanged
        Qt.callLater(updateValue)
    }
}
