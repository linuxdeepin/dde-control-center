// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes
import org.deepin.dtk 1.0 as D

Control {
    id: control
    property real radius: 8
    property bool invalidFileType: false
    implicitWidth: 460
    implicitHeight: 360

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
            fillColor: {
                if (dropArea.containsDrag) {
                    Qt.rgba(0, 0, 0, 0.08)
                } else if (mouseArea.pressed) {
                    Qt.rgba(0, 0, 0, 0.10)
                } else if (mouseArea.containsMouse) {
                    Qt.rgba(0, 0, 0, 0.05)
                } else {
                    palette.window
                }
            }
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
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                control.invalidFileType = false
                requireFileDialog()
            }
        }
        DropArea {
            id: dropArea
            anchors.fill: parent
            enabled: true
            onDropped: function (drop) {
                if (drop.hasUrls) {
                    var filePath = drop.urls[0].toString()
                    var ext = filePath.substring(filePath.lastIndexOf('.') + 1).toLowerCase()
                    if (['png', 'bmp', 'jpg', 'jpeg'].indexOf(ext) === -1) {
                        control.invalidFileType = true
                        return
                    }
                    control.invalidFileType = false
                    iconDropped(filePath)
                }
            }
        }
    }

    Item {
        id: emptyState
        width: shape.width - 40
        height: addIcon.height + 20 + messageLabel.implicitHeight
        anchors.centerIn: shape

        D.DciIcon {
            id: addIcon
            width: 64
            height: 64
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            name: "dcc_user_add_icon"
            fallbackToQIcon: false
            palette: D.DTK.makeIconPalette(control.palette)
            mode: control.D.ColorSelector.controlState
            theme: control.D.ColorSelector.controlTheme
            sourceSize: Qt.size(width, height)
        }

        D.Label {
            id: messageLabel
            width: parent.width
            wrapMode: Text.WordWrap
            font: D.DTK.fontManager.t8
            anchors.top: addIcon.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            text: control.invalidFileType
                ? qsTr("The uploaded file type is incorrect, please upload it again")
                : qsTr("You haven't uploaded an avatar yet. Click or drag and drop to upload an image.")
        }
    }
}
