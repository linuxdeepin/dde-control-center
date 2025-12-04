// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import org.deepin.dtk 1.0 as D
import Qt.labs.platform 1.1

Control {
    id: control
    property string iconSource
    property int cropSize: 120
    property alias dragActived: imgContainter.dragActived
    property alias imgScale: img2.scale
    property string previewUrl: ""
    width: 430
    height: 240

    signal croppedImage(string file)
    signal previewUpdated(string previewFile)

    RowLayout {
        id: layout
        height: 190
        Layout.fillWidth: true
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle {
            id: imgGrabber
            width: control.cropSize
            height: control.cropSize
            clip: true
            visible: false

            Image {
                id: sourceItem
                width: img2.width
                height: img2.height
                source: control.iconSource
                cache: false
            }
        }

        Rectangle {
            id: imgContainter
            property bool dragActived: false
            width: control.iconSource.length > 0 ? 190 : 120
            height: width
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            clip: true
            color: control.iconSource.length > 0 ? "transparent" : control.palette.button
            radius: control.iconSource.length > 0 ? 0 : 18

            Image {
                id: img2
                property bool initialAligned: false
                // Fit strategy: fix height to cropSize, width by aspect ratio
                height: control.cropSize
                width: height * (sourceSize.height > 0 ? (sourceSize.width / sourceSize.height) : 1)
                x: (imgContainter.width - width) / 2
                y: (imgContainter.height - height) / 2
                source: control.iconSource
                cache: false
                onStatusChanged: {
                    if (status === Image.Ready) {
                        initialAligned = false
                        img2.updatePos()
                    }
                }
                onSourceChanged: initialAligned = false
                Component.onCompleted: img2.updatePos()

                function updatePos() {
                    // Compute crop rect (fixed size cropSize centered in container)
                    let cropStartX = (imgContainter.width - control.cropSize) / 2
                    let cropStartY = (imgContainter.height - control.cropSize) / 2
                    let cropEndX = cropStartX + control.cropSize
                    let cropEndY = cropStartY + control.cropSize

                    // Recompute size based on source aspect ratio
                    let aspect = 1.0
                    if (img2.sourceSize.height > 0) {
                        aspect = img2.sourceSize.width / img2.sourceSize.height
                        if (aspect >= 1.0) {
                            // Landscape: fit height, width by aspect
                            img2.height = control.cropSize
                            img2.width = control.cropSize * aspect
                        } else {
                            // Portrait: fit width, height by aspect
                            img2.width = control.cropSize
                            img2.height = control.cropSize / aspect
                        }
                    }

                    let offsetX = (img2.scale - 1.0) * img2.width / 2
                    let offsetY = (img2.scale - 1.0) * img2.height / 2

                    // First time after image ready: center horizontally/vertically
                    if (!initialAligned) {
                        img2.x = (imgContainter.width - img2.width) / 2
                        img2.y = (imgContainter.height - img2.height) / 2
                        initialAligned = true
                    }

                    let visualX = img2.x - offsetX
                    let visualY = img2.y - offsetY
                    let visualW = img2.scale * img2.width
                    let visualH = img2.scale * img2.height

                    // Constrain both axes so that the scaled image always covers the crop square
                    if (visualX > cropStartX) {
                        img2.x = offsetX + cropStartX
                    } else if (visualX + visualW < cropEndX) {
                        img2.x = cropEndX - visualW + offsetX
                    }
                    if (visualY > cropStartY) {
                        img2.y = offsetY + cropStartY
                    } else if (visualY + visualH < cropEndY) {
                        img2.y = cropEndY - visualH + offsetY
                    }

                    // Sync hidden grab source to crop rect origin
                    sourceItem.scale = img2.scale
                    sourceItem.x = img2.x - cropStartX
                    sourceItem.y = img2.y - cropStartY
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
                    onTranslationChanged: img2.updatePos()
                }
            }

            MouseArea {
                anchors.fill: parent
                onPressed: function(mouse) {
                    mouse.accepted = false
                    boxShadow.shadowColor = "#88000000"
                    imgContainter.dragActived = true
                    timer.restart()
                }
                onWheel: function(wheel) {
                    img2.updatePos()
                }
            }

            Timer {
                id: timer
                interval: 1000;
                onTriggered: {
                    boxShadow.shadowColor = palette.window
                    imgContainter.dragActived = false
                    control.updatePreview()
                }
            }

            D.BoxShadow {
                id: boxShadow
                hollow: true
                anchors.fill: parent
                anchors.margins: Math.max(0, (imgContainter.width - control.cropSize) / 2)
                shadowBlur: 36
                spread: 80
                shadowColor: palette.window
                cornerRadius: 18
            }
        }
    }

    RowLayout {
        anchors.top: layout.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        D.Label {
            text: qsTr("small")
            font: D.DTK.fontManager.t8
            verticalAlignment: Text.AlignVCenter
            Layout.alignment: Qt.AlignVCenter
        }

        Slider {
            id: scaleSlider
            from: 1.0
            to: 2.0
            stepSize: 0.5
            value: img2.scale
            implicitHeight: 24
            handleType: Slider.HandleType.ArrowBottom
            Layout.alignment: Qt.AlignVCenter
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

        D.Label {
            text: qsTr("big")
            font: D.DTK.fontManager.t8
            verticalAlignment: Text.AlignVCenter
            Layout.alignment: Qt.AlignVCenter
        }
    }

    function cropToTemp(callback) {
        if (!control.iconSource || control.iconSource.length < 1) {
            if (callback) callback("")
            return
        }
        imgGrabber.grabToImage(function (result) {
            const tmp = StandardPaths.writableLocation(StandardPaths.TempLocation) + "/dcc_avatar_tmp.png"
            result.saveToFile(tmp)
            if (callback) callback(tmp)
        })
    }

    property var _tempPreviewFiles: []

    function updatePreview() {
        if (!control.iconSource || control.iconSource.length < 1) {
            control.previewUrl = ""
            return
        }
        imgGrabber.grabToImage(function (result) {
            let tmpDir = StandardPaths.writableLocation(StandardPaths.TempLocation).toString()
            tmpDir = tmpDir.replace("file://", "")
            const tmp = tmpDir + "/dcc_avatar_preview_" + Date.now() + ".png"
            result.saveToFile(tmp)
            control._tempPreviewFiles.push(tmp)
            control.previewUrl = "file://" + tmp
            control.previewUpdated(control.previewUrl)
        })
    }

    function getTempPreviewFiles() {
        return control._tempPreviewFiles
    }

    function clearTempPreviewFiles() {
        control._tempPreviewFiles = []
        control.previewUrl = ""
    }
}
