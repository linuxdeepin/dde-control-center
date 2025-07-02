// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Effects
import org.deepin.dtk 1.0 as D

Rectangle {
    id: root
    property var screen
    property real translationX: 100
    property real translationY: 100
    property real scale: 0.1
    property bool selected: false
    signal pressed
    signal positionChanged
    signal released
    signal updatePosition

    color: "#5f5f5f"
    radius: width * 0.05
    focus: true

    Image {
        id: image
        anchors.fill: parent
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
    }
    Text {
        x: parent.radius + 5
        y: parent.radius + 5
        text: screen.name
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
        visible: screen && dccData.primaryScreen && (screen.name === dccData.primaryScreen.name)
        name: "home_screen"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: parent.radius + 5
        anchors.rightMargin: parent.radius + 5
        sourceSize: Qt.size(24, 24)
    }
    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: "transparent"
        border.color: root.selected ? "#2ca7f8" : "#802e2e2e"
        border.width: 2
        smooth: true
    }
    MouseArea {
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
