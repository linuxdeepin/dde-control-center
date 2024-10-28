// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes
import org.deepin.dtk 1.0 as D

Control {
    id: control
    property real radius: 8
    width: 430
    height: 360

    signal requireFileDialog()
    signal iconDropped(string file)

    Shape {
        id: shape
        anchors.centerIn: parent
        width: 400
        height: 300
        layer.enabled: true
        layer.samples: 4
        layer.smooth: true

        ShapePath {
            strokeStyle: ShapePath.DashLine
            strokeWidth: 1
            fillColor: palette.window
            strokeColor: "gray"
            startX: control.radius
            startY: 0

            PathLine { x: shape.width - control.radius; y: 0 }
            PathQuad { x: shape.width; y: control.radius; controlX: shape.width; controlY: 0 }
            PathLine { x: shape.width; y: shape.height - control.radius }
            PathQuad { x: shape.width - control.radius; y: shape.height; controlX: shape.width; controlY: shape.height }
            PathLine { x: control.radius; y: shape.height }
            PathQuad { x: 0; y: shape.height - control.radius; controlX: 0; controlY: shape.height }
            PathLine { x: 0; y: control.radius }
            PathQuad { x: control.radius; y: 0; controlX: 0; controlY: 0 }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                requireFileDialog()
            }
        }
        DropArea {
            anchors.fill: parent
            enabled: true
            onDropped: function (drop) {
                if (drop.hasUrls) {
                    iconDropped(drop.urls[0])
                }
            }
        }
    }
    D.DciIcon {
        id: addIcon
        anchors.centerIn: parent
        name: "dcc_user_add_icon"
        sourceSize:  Qt.size(64, 64)
    }

    D.Label {
        width: 360
        wrapMode: Text.WordWrap
        anchors.top: addIcon.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("You haven't uploaded an avatar yet. Click or drag and drop to upload an image.")
    }
}
