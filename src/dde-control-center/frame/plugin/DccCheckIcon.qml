// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    property real size: 16
    property bool checked: true
    signal clicked

    implicitHeight: size
    implicitWidth: size
    radius: size / 2
    color: checked ? root.palette.highlight : "transparent"
    border {
        color: root.palette.text
        width: checked ? 0 : 1
    }
    Image {
        anchors.fill: parent
        visible: checked
        sourceSize.width: size
        sourceSize.height: size
        source: "dcc_checked"
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onClicked: root.clicked()
    }
}
