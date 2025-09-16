// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

D.DialogWindow {
    id: selectDayDialog
    modality: Qt.ApplicationModal
    width: 330
    height: 420

    signal cancel()
    signal accept()

    // Copy is used here to prevent contamination of data in the original model when selecting items
    property var selectedDays: []
    property var dateStr: {
        var locale = Qt.locale(Qt.locale().name);
        var days = [];
        for (var i = 1; i <= 7; i++) {
            var dayEnum = Qt.Monday + (i - 1) % 7;
            days.push(locale.standaloneDayName(dayEnum, "long"));
        }
        return days;
    }

    // 当对话框显示时，重新同步最新的自定义重复时间数据
    function syncSelectedDays() {
        selectedDays = dccData.model.customShutdownWeekDays.length === 0 ? [1, 2, 3, 4, 5] : dccData.model.customShutdownWeekDays.slice()
    }

    // 当对话框可见性改变时，同步数据
    onVisibleChanged: {
        if (visible) {
            syncSelectedDays()
        }
    }

    property var dayModel: generateDayModel()

    function generateDayModel() {
        var array = []
        let weekBegin = dccData.model.weekBegins
        if (weekBegin < 1 || weekBegin > 7) {
            // default is monday
            weekBegin = 1
        }
        for (var i = dccData.model.weekBegins; i <= 7; i++) {
            array.push(i)
        }
        for (i = 1; i < dccData.model.weekBegins; i++) {
            array.push(i)
        }
        return array
    }

    // only accept close event , the app can quit normally
    onClosing: function(close) {
        close.accepted = true
        // 当用户点击关闭按钮或按ESC键关闭对话框时，触发取消事件
        syncSelectedDays() // 重新同步数据
        selectDayDialog.cancel()
    }

    ColumnLayout {
        implicitWidth: parent.width
        clip: true
        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Customize repetition time")
        }
        ListView {
            Layout.fillWidth: true
            height: contentHeight
            clip: true
            model: selectDayDialog.dayModel
            spacing: 2
            delegate: D.ItemDelegate {
                width: ListView.view.width
                leftPadding: 10
                rightPadding: 10
                implicitHeight: 40
                checkable: false
                corners: getCornersForBackground(index, 7)
                cascadeSelected: true
                text: selectDayDialog.dateStr[modelData - 1]
                onClicked: handleSelected(modelData)
                content: DccCheckIcon {
                    checked: selectDayDialog.selectedDays.indexOf(modelData) !== -1
                    onClicked: handleSelected(modelData)
                }
                background: DccItemBackground {
                    separatorVisible: true
                }

                function handleSelected(index) {
                    if (selectDayDialog.selectedDays.indexOf(index) === -1) {
                        selectDayDialog.selectedDays.push(index);
                    } else {
                        selectDayDialog.selectedDays.splice(selectDayDialog.selectedDays.indexOf(index), 1);
                    }
                    selectDayDialog.selectedDays = selectDayDialog.selectedDays.slice();
                }
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            D.Button {
                text: qsTr("Cancel")
                onClicked: {
                    selectDayDialog.close()
                    syncSelectedDays() // 重新同步数据
                    selectDayDialog.cancel()
                }
            }
            D.Button {
                text: qsTr("Save")
                enabled: selectDayDialog.selectedDays.length > 0
                onClicked: {
                    var days = selectDayDialog.selectedDays.sort()
                    console.log("Selected days: " + days);
                    dccData.worker.setCustomShutdownWeekDays(days)
                    selectDayDialog.close()
                    selectDayDialog.accept()
                }
            }
        }
    }
}
