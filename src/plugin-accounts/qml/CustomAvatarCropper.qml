// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import org.deepin.dtk 1.0 as D

Control {
    id: control
    property string iconSource
    property alias dragActived: imgContainter.dragActived
    property alias imgScale: img2.scale
    width: 430
    height: 240

    signal croppedImage(string file)

    RowLayout {
        id: layout
        height: 200
        Layout.fillWidth: true
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle {
            id: imgGrabber
            width: 200
            height: 200
            clip: true
            visible: false

            Image {
                id: sourceItem
                width: 200
                height: 200
                source: control.iconSource
            }
        }

        Rectangle {
            id: imgContainter
            property bool dragActived: false
            width: control.iconSource.length > 0 ? 200 : 100
            height: width
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            clip: true
            color: control.iconSource.length > 0 ? "transparent" : control.palette.button
            radius: control.iconSource.length > 0 ? 0 : 18

            Image {
                id: img2
                width: 100
                height: 100
                source: control.iconSource

                function updatePos() {
                    let start = 50
                    let end = start + img2.width
                    let offset = (img2.scale - 1.0) * img2.width / 2

                    let visualX = img2.x - offset
                    let visualY = img2.y - offset
                    let visualSize = img2.scale * img2.width

                    if (visualX > start) {
                        img2.x = offset + start
                    } else if (visualX + visualSize < end) {
                        img2.x = end - visualSize + offset
                    } else {
                        // valid pos
                    }

                    if (visualY > start) {
                        img2.y = offset + start
                    } else if (visualY + visualSize < end) {
                        img2.y = end - visualSize + offset
                    } else {
                        // valid pos
                    }

                    sourceItem.scale = img2.scale

                    sourceItem.x = img2.x - 50
                    sourceItem.y = img2.y - 50
                }

                DragHandler {
                    id: dragHandler
                    onActiveChanged: {
                        if (active) {
                            timer.stop()
                        } else {
                            img2.updatePos()
                            timer.restart()
                        }
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onPressed: function(mouse) {
                    mouse.accepted = false
                    // boxShadow.visible = true
                    boxShadow.shadowColor = "#88000000"
                    imgContainter.dragActived = true
                    timer.restart()
                }
            }

            Timer {
                id: timer
                interval: 1000;
                onTriggered: {
                    // boxShadow.visible = false
                    // fake hide...
                    boxShadow.shadowColor = palette.window
                    imgContainter.dragActived = false
                    imgGrabber.grabToImage(function (result) {
                        result.saveToFile("/tmp/dcc_avatar.png")
                        croppedImage("/tmp/dcc_avatar.png")
                    })
                }
            }

            D.BoxShadow {
                id: boxShadow
                hollow: true
                anchors.fill: parent
                anchors.margins: 50
                shadowBlur: 36
                spread: 80
                shadowColor: palette.window
                cornerRadius: 18
            }
        }
    }

    Slider {
        anchors.top: layout.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        from: 1.0
        to: 2.0
        stepSize: 0.5
        value: img2.scale
        handleType: Slider.HandleType.ArrowBottom
        onValueChanged: {
            img2.scale = value
            img2.updatePos()
        }
        onPositionChanged: {
            boxShadow.shadowColor = "#88000000"
            imgContainter.dragActived = true
            timer.restart()
        }
    }
}
