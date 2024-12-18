// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

Item {
    id: clock

    property int hours
    property int minutes
    property real shift: 8 // (UTC+08)
    property bool internationalTime: true
    property bool animationEnabled: false

    width: 80
    height: 80

    Component.onCompleted: {
        tick()
    }

    function tick() {
        const date = new Date
        clock.hours = internationalTime ? date.getUTCHours() + Math.floor(clock.shift) : date.getHours()
        clock.minutes = internationalTime ? date.getUTCMinutes() + ((clock.shift % 1) * 60) : date.getMinutes()
        if (clock.hours < 0)
            clock.hours += 12
    }

    Timer {
        id: timer
        interval: 800
        running: true
        repeat: true
        onTriggered: clock.tick()
    }

    Item {
        id: clockPanel
        anchors.fill: parent

        Rectangle {
            id: background
            anchors.centerIn: parent
            radius: width / 2
            height: clockPanel.height
            width: clockPanel.width
            color: (hours >= 6 && hours < 18) ? "#E1E1E1" : "#575757"
        }

        Rectangle {
            id: hourHandle
            width: background.width / 3
            height: 4
            radius: height / 2
            x: background.x + (background.width) / 2 - 2
            y: background.y + background.height / 2 - 2
            color: "#07c5fb"
            antialiasing: true
            transform: Rotation {
                id: hourRotation
                origin.x: 2
                origin.y: 2
                angle: (clock.hours * 30) + (clock.minutes * 0.5) - 90
                Behavior on angle {
                    enabled: clock.animationEnabled
                    SpringAnimation {
                        spring: 2
                        damping: 0.2
                        modulus: 360
                    }
                }
            }
        }

        Rectangle {
            id: minuteHandle
            color: "#f97676"
            height: 4
            radius: height / 2
            width: background.width * 0.4
            x: background.x + background.width / 2 - 2
            y: background.y + background.height / 2 - 2
            antialiasing: true
            transform: Rotation {
                id: minuteRotation
                origin.x: 2
                origin.y: 2
                angle: clock.minutes * 6 - 90
                Behavior on angle {
                    enabled: clock.animationEnabled
                    SpringAnimation {
                        spring: 2
                        damping: 0.2
                        modulus: 360
                    }
                }
            }
        }
    }
}
