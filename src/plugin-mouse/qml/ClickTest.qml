// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Item {
    id: root
    property var outAni: []
    property var inAni: []
    property bool isOut: true
    property int range: 0
    property int index: 0
    Image {
        id: doubleClickAni
        source: "qrc:/mouse/double_test/raise_head/raise_head_00001.png"
    }

    Timer {
        id: timer
        interval: 50
        repeat: true
        onTriggered: {
            if (isOut) {
                doubleClickAni.source = outAni[index]
            } else {
                doubleClickAni.source = inAni[index]
            }
            index = index + 1
            if (index >= range) {
                index = 0
                timer.stop()
                isOut = !isOut
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onDoubleClicked: {
            if (timer.running)
                isOut = !isOut
            if (isOut) {
                range = 37
            } else {
                range = 19
            }
            index = 0
            timer.restart()
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < 38; i++) {
            outAni[i] = "qrc:/mouse/double_test/raise_head/raise_head_000" + (i + 1).toString().padStart(2, '0') + ".png"
            // console.log(outAni[i])
        }
        for (var j = 0; j < 20; j++) {
            inAni[j] = "qrc:/mouse/double_test/bow_head/bow_head_000" + (j + 1).toString().padStart(2, '0') + ".png"
            // console.log(inAni[j])
        }
    }
}
