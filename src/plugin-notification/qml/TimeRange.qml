// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

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
