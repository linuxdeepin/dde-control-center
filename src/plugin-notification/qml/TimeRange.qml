// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import org.deepin.dtk as D
import org.deepin.dcc

RowLayout {
    property var sysItemModel: dccData.sysItemModel
    spacing: 5
    D.CheckBox {
        id: timeSlotCheckBox
        implicitHeight: implicitContentHeight + 30
        checked: dccData.sysItemModel.timeSlot
        onCheckedChanged: {
            if (dccData.sysItemModel.timeSlot !== checked) {
                dccData.sysItemModel.timeSlot = checked
            }
        }
    }
    D.Label {
        text: qsTr("from")
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onClicked: {
                timeSlotCheckBox.checked = !timeSlotCheckBox.checked
            }
        }
    }
    DccTimeRange {
        id: hourTime
        hour: sysItemModel.timeStart.split(":")[0]
        minute: sysItemModel.timeStart.split(":")[1]
        onTimeChanged: sysItemModel.timeStart = timeString
    }
    D.Label {
        text: qsTr("to")
    }
    DccTimeRange {
        hour: sysItemModel.timeEnd.split(":")[0]
        minute: sysItemModel.timeEnd.split(":")[1]
        onTimeChanged: sysItemModel.timeEnd = timeString
    }
    Item {
        Layout.fillWidth: true
    }
}
