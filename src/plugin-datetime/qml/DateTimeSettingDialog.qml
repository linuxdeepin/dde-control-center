// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
        
D.DialogWindow {
id: ddialog
    width: 400
    height: 220
    icon: "preferences-system"
    modality: Qt.WindowModal
    property date currentDate: new Date()

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
                unitText: qsTr("年")
                from: 1900
                to: 9999
                value: currentDate.getFullYear()
                onValueChanged: {
                    currentDate.setFullYear(value);
                }
            }

            SpinboxEx {
                unitText: qsTr("月")
                from: 1
                to: 12
                value: currentDate.getMonth() + 1 //  // January gives 0
                onValueChanged: {
                    currentDate.setMonth(value - 1)
                }
            }
            SpinboxEx {
                unitText: qsTr("日")
                from: 1
                to: 31
                value: currentDate.getDate()
                onValueChanged: {
                    currentDate.setDate(value)
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
                from: 0
                to: 23
                value: currentDate.getHours()
                onValueChanged: {
                    currentDate.setHours(value);
                }
            }
            SpinboxEx {
                from: 0
                to: 59
                value: currentDate.getMinutes()
                onValueChanged: {
                    currentDate.setMinutes(value);
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
                    dccData.setDateTime(currentDate)
                    ddialog.close()
                }
            }
        }
    }
}
    
