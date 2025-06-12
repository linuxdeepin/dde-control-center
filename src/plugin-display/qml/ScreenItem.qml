// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import Qt5Compat.GraphicalEffects
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
        source: screen.wallpaper
        mipmap: true
        visible: false
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        cache: true
        smooth: true
        sourceSize {
            width: parent.width * Screen.devicePixelRatio
            height: parent.height * Screen.devicePixelRatio
        }
        onStatusChanged: {
            if (status === Image.Loading) {
                loadingPlaceholder.visible = true
            } else {
                loadingPlaceholder.visible = false
            }
        }
    }

    Rectangle {
        id: loadingPlaceholder
        anchors.fill: parent
        color: "#3f3f3f"
        visible: false
        radius: root.radius
    }
    OpacityMask {
        anchors.fill: parent
        source: image
        maskSource: Rectangle {
            implicitWidth: image.width
            implicitHeight: image.height
            radius: root.radius
        }
    }
    Label {
        anchors.fill: parent
        anchors.topMargin: parent.radius + 5
        anchors.leftMargin: parent.radius + 5
        wrapMode: Text.Wrap
        // horizontalAlignment: Text.AlignHCenter
        // verticalAlignment: Text.AlignVCenter
        text: screen.name
        color: "white"
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
