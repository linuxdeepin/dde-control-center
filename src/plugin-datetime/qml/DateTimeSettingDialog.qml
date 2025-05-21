// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
        
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
                onValueChanged: ddialog.updateDateMax()
                Component.onCompleted: {
                    let year = currentDate.getFullYear()
                    spYear.from = year - 30
                    spYear.to = year + 30
                }
            }
            SpinboxEx {
                id: spMonth
                unitText: qsTr("Month")
                from: 1
                to: 12
                wrap: true
                value: currentDate.getMonth() + 1 //  // January gives 0
                onValueChanged: ddialog.updateDateMax()
            }
            SpinboxEx {
                id: spDay
                unitText: qsTr("Day")
                from: 1
                to: 31
                wrap: true
                value: currentDate.getDate()
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
            }
            SpinboxEx {
                id: spMin
                from: 0
                to: 59
                wrap: true
                value: currentDate.getMinutes()
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
    
