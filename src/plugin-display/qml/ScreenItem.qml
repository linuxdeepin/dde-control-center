// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Effects
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

Item {
    id: root
    property var screen
    property real translationX: 100
    property real translationY: 100
    property real scale: 0.1
    property real radius: width * 0.05
    property bool selected: false
    property real offset: (imgRepeater.count - 1) * 6
    signal pressed
    signal positionChanged
    signal released
    signal updatePosition

    focus: true
    Repeater {
        id: imgRepeater
        model: screen.screenItems.length
        delegate: Image {
            id: image
            x: offset - index * 6
            y: offset - index * 6
            z: 1 - (0.01 * index)
            width: root.width - offset
            height: root.height - offset
            opacity: index != 0 ? 0.8 : 1
            source: "image://DccImage/" + screen.wallpaper
            mipmap: true
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
            layer.enabled: true
            layer.effect: MultiEffect {
                maskEnabled: true
                maskSource: imageMask
                antialiasing: true
                maskThresholdMin: 0.5
                maskSpreadAtMin: 1.0
            }
            Item {
                id: imageMask
                anchors.fill: parent
                layer.enabled: true
                visible: false
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 0.5
                    radius: root.radius
                }
            }
            Rectangle {
                anchors.fill: parent
                radius: root.radius
                color: "transparent"
                opacity: (index === 0 && root.selected) ? 0.7 : 0.2
                border.color: (index === 0 && root.selected) ? this.palette.window : "#000000"
                border.width: 1
                smooth: true
            }
        }
    }
    DccLabel {
        x: offset
        y: offset
        z: 2
        width: root.width - offset
        height: root.height - offset
        padding: root.radius + 5
        text: screen.name
        elide: Text.ElideMiddle
        color: "white"
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.01
            shadowColor: Qt.rgba(0.0, 0.0, 0.0, 0.7)
            shadowVerticalOffset: 1
        }
    }
    D.DciIcon {
        z: 2
        visible: screen && dccData.primaryScreen && (screen.name === dccData.primaryScreen.name)
        name: "home_screen"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: parent.radius + 5
        anchors.rightMargin: parent.radius + 5
        sourceSize: Qt.size(24, 24)
    }
    Loader {
        x: offset
        y: offset
        z: 2
        width: root.width - offset
        height: root.height - offset
        active: root.selected
        sourceComponent: Rectangle {
            anchors.fill: parent
            radius: root.radius + 1
            color: "transparent"
            border.color: this.palette.highlight
            border.width: 1
            smooth: true
        }
    }
    MouseArea {
        z: 2
        anchors.fill: parent
        drag.target: parent
        onPressed: root.pressed()
        onPositionChanged: root.positionChanged()
        onReleased: root.released()
    }
    Component.onCompleted: updatePosition()

    Connections {
        target: screen
        function onXChanged() {
            updatePosition()
        }
        function onYChanged() {
            updatePosition()
        }
        function onWidthChanged() {
            updatePosition()
        }
        function onHeightChanged() {
            updatePosition()
        }
    }
}
