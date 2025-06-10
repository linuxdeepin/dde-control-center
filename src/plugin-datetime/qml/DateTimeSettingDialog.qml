// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
        
D.DialogWindow {
    id: ddialog
    width: 400
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    icon: "preferences-system"
    modality: Qt.WindowModal
    property date currentDate: new Date()

    component SpinboxTextInput: TextInput {
        property var spinbox: parent

        text: spinbox.displayText
        font: spinbox.font
        color: spinbox.palette.text
        selectionColor: spinbox.palette.highlight
        selectedTextColor: spinbox.palette.highlightedText
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter
        leftPadding: DS.Style.spinBox.spacing
        readOnly: !spinbox.editable
        validator: spinbox.validator
        inputMethodHints: spinbox.inputMethodHints
        selectByMouse: spinbox.editable

        onActiveFocusChanged: {
            if (!activeFocus && text === "") {
                text = spinbox.value
            }
        }

        onTextEdited: {
            if (text === "") return;

            let value = parseInt(text);
            if (spinbox.from > 0 && text.length === 1 && text === "0") {
                text = "";
                return;
            }
            if (value > spinbox.to) {
                text = text.substring(0, text.length - 1);
                return;
            }

            let minDigits = Math.floor(Math.log10(spinbox.from)) + 1;
            let maxDigits = Math.floor(Math.log10(spinbox.to)) + 1;
            let currentLength = text.length;

            let minPossible;
            if (currentLength >= minDigits) {
                minPossible = value;
            } else {
                minPossible = parseInt(text + "0".repeat(minDigits - currentLength));
            }

            let maxPossible;
            if (currentLength >= maxDigits) {
                maxPossible = value;
            } else {
                maxPossible = parseInt(text + "9".repeat(maxDigits - currentLength));
            }

            let isValid = (minPossible <= spinbox.to && maxPossible >= spinbox.from);
            if (!isValid) {
                text = text.substring(0, text.length - 1);
            } else if (value >= spinbox.from && value <= spinbox.to) {
                spinbox.value = value;
            }
        }
    }

    function getDaysInMonth(year, month) {
        return new Date(year, month, 0).getDate()
    }

    function updateDateMax() {
        spDay.to = getDaysInMonth(spYear.value, spMonth.value)
    }

    ColumnLayout {
        spacing: 10
        width: parent.width
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.bold: true
            text: qsTr("Date and time setting")
        }
        RowLayout {
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 20
            Label {
                font: D.DTK.fontManager.t5
                text: qsTr("Date")
            }
            SpinboxEx {
                id: spYear
                unitText: qsTr("Year")
                locale: Qt.locale("C")
                from: 1990
                to: 2090
                wrap: true
                value: currentDate.getFullYear()
                onValueChanged: {
                    ddialog.updateDateMax()
                    yearTextInput.text  = value
                }
                Component.onCompleted: {
                    let year = currentDate.getFullYear()
                    spYear.from = year - 30
                    spYear.to = year + 30
                }

                contentItem: SpinboxTextInput {
                    id: yearTextInput
                    spinbox: spYear
                }
            }
            SpinboxEx {
                id: spMonth
                unitText: qsTr("Month")
                from: 1
                to: 12
                wrap: true
                value: currentDate.getMonth() + 1 //  // January gives 0
                onValueChanged: {
                    ddialog.updateDateMax()
                    monthTextInput.text = value
                }

                contentItem: SpinboxTextInput {
                    id: monthTextInput
                    spinbox: spMonth
                }
            }
            SpinboxEx {
                id: spDay
                unitText: qsTr("Day")
                from: 1
                to: 31
                wrap: true
                value: currentDate.getDate()
                onValueChanged: {
                    dayTextInput.text = value
                }

                contentItem: SpinboxTextInput {
                    id: dayTextInput
                    spinbox: spDay
                }
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 20
            Label {
                font: D.DTK.fontManager.t5
                text: qsTr("Time")
            }
            SpinboxEx {
                id: spHour
                from: 0
                to: 23
                wrap: true
                value: currentDate.getHours()
                onValueChanged: {
                    hourTextInput.text = value
                }

                contentItem: SpinboxTextInput {
                    id: hourTextInput
                    spinbox: spHour
                }
            }
            SpinboxEx {
                id: spMin
                from: 0
                to: 59
                wrap: true
                value: currentDate.getMinutes()
                onValueChanged: {
                    minTextInput.text = value
                }

                contentItem: SpinboxTextInput {
                    id: minTextInput
                    spinbox: spMin
                }
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            Button {
                Layout.bottomMargin: 10
                text: qsTr("Cancel")
                onClicked: {
                    ddialog.close()
                }
            }
            Button {
                Layout.bottomMargin: 10
                text: qsTr("Confirm")
                onClicked: {
                    let dateTime = currentDate
                    dateTime.setFullYear(spYear.value)
                    dateTime.setMonth(spMonth.value - 1)
                    dateTime.setDate(spDay.value)
                    dateTime.setHours(spHour.value)
                    dateTime.setMinutes(spMin.value)
                    dateTime.setSeconds(0)

                    dccData.setDateTime(dateTime)
                    ddialog.close()
                }
            }
        }
    }
}
    
