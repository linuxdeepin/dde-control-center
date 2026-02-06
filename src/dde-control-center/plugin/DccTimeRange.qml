// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.SpinBox {
    id: control
    wrap: true
    readonly property string timeString: textFromValue(value)
    property int hour: 0
    property int minute: 0
    property int initialValue: 0
    signal timeChanged

    Layout.maximumWidth: 110
    
    Timer {
        id: valueChangeTimer
        interval: 500  // 500ms 防抖，用户停止操作后才发送
        onTriggered: {
            if (control.value !== control.initialValue) {
                control.timeChanged()
                control.initialValue = control.value  // 更新初始值
            }
        }
    }
    from: 0
    to: 1380 // 23 hours * 60 minutes
    stepSize: 60
    value: hour * 60 + minute
    editable: true
    font: D.DTK.fontManager.t7
    inputMethodHints: Qt.ImhDigitsOnly

    function formatText(value) {
        return value < 10 ? "0" + value : value
    }
    onHourChanged: {
        var m = value % 60
        if (stepSize === 1) {
            from = hour * 60
            to = hour * 60 + 59
        }
        value = hour * 60 + m
    }
    onMinuteChanged: {
        var h = Math.floor(value / 60)
        if (stepSize === 60) {
            from = minute
            to = 23 * 60 + minute
        }
        value = h * 60 + minute
    }

    textFromValue: function (value) {
        var hours = Math.floor(value / 60)
        var minutes = value % 60
        return (hours < 10 ? "0" + hours : hours) + ":" + (minutes < 10 ? "0" + minutes : minutes)
    }
    valueFromText: function (text) {
        var parts = text.split(":")
        var hours = parseInt(parts[0], 10)
        var minutes = parseInt(parts[1], 10)
        switch (stepSize) {
        case 1:
            control.from = hours * 60
            control.to = hours * 60 + 59
            break
        case 60:
            control.from = minutes
            control.to = 23 * 60 + minutes
            break
        }
        return hours * 60 + minutes
    }
    contentItem: RowLayout {
        property string text: hourInput.text + ":" + minuteInput.text
        spacing: 0
        opacity: enabled ? 1 : 0.4
        TextInput {
            id: hourInput
            property bool typingDigit: false
            Layout.preferredWidth: 20
            Layout.alignment: Qt.AlignHCenter
            text: control.formatText(Math.floor(value / 60))
            font: control.font
            color: control.palette.text
            selectionColor: control.palette.highlight
            selectedTextColor: control.palette.highlightedText
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            leftPadding: DS.Style.spinBox.spacing
            readOnly: !control.editable
            validator: RegularExpressionValidator {
                regularExpression: /^(?:[0-1]?[0-9]|2[0-3])$/
            }
            maximumLength: 2
            inputMethodHints: control.inputMethodHints
            selectByMouse: true
            mouseSelectionMode: TextInput.SelectCharacters
            onFocusChanged: {
                if (focus) {
                    control.stepSize = 60
                    var minutes = value % 60
                    control.from = minutes
                    control.to = 23 * 60 + minutes
                } else {
                    typingDigit = false
                }
            }
            Keys.onPressed: function(event) {
                if (event.key >= Qt.Key_0 && event.key <= Qt.Key_9) {
                    typingDigit = true
                }
            }
            onTextChanged: {
                if (focus && typingDigit) {
                    if (text.length === 1) {
                        var firstDigit = parseInt(text, 10)
                        if (firstDigit >= 3) {
                            typingDigit = false
                            minuteInput.forceActiveFocus()
                            minuteInput.selectAll()
                        }
                    }
                    else if (text.length === 2) {
                        typingDigit = false
                        minuteInput.forceActiveFocus()
                        minuteInput.selectAll()
                    }
                }
            }
            Keys.onRightPressed: function(event) {
                if (cursorPosition === length) {
                    minuteInput.forceActiveFocus()
                    minuteInput.selectAll()
                    event.accepted = true
                } else {
                    event.accepted = false
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
            text: control.formatText(Math.floor(value % 60))
            font: control.font
            color: control.palette.text
            selectionColor: control.palette.highlight
            selectedTextColor: control.palette.highlightedText
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            leftPadding: DS.Style.spinBox.spacing
            readOnly: !control.editable
            validator: RegularExpressionValidator {
                regularExpression: /^(?:[0-5]?[0-9])$/
            }
            maximumLength: 2
            inputMethodHints: control.inputMethodHints
            selectByMouse: true
            mouseSelectionMode: TextInput.SelectCharacters
            onFocusChanged: {
                if (focus) {
                    control.stepSize = 1
                    var hours = Math.floor(value / 60)
                    control.from = hours * 60
                    control.to = hours * 60 + 59
                }
            }
            Keys.onLeftPressed: function(event) {
                if (cursorPosition === 0) {
                    hourInput.forceActiveFocus()
                    hourInput.selectAll()
                    event.accepted = true
                } else {
                    event.accepted = false
                }
            }
        }
    }
    onValueChanged: {
        valueChangeTimer.restart()
    }
    onActiveFocusChanged: {
        if (activeFocus) {
            valueChangeTimer.stop()
            initialValue = value
        } else {
            valueChangeTimer.restart()
        }
    }
}
