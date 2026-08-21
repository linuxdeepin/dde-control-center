// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia

import org.deepin.dtk as D

Rectangle {
    id: root
    property string sourcePath: ""

    color: "transparent"
    clip: true
    radius: root.radius

    // TODO
    readonly property bool isVideo:
        sourcePath.endsWith(".mp4") ||
        sourcePath.endsWith(".mkv") ||
        sourcePath.endsWith(".mov") ||
        sourcePath.endsWith(".webm")

    Image {
        id: imageItem
        anchors.fill: parent
        opacity: root.isVideo ? 0 : 1
        source: root.isVideo ? "" : "image://DccImage/" + root.sourcePath
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        mipmap: true
        retainWhileLoading: true
    }

    MediaPlayer {
        id: mediaplayer
        source: root.isVideo ? root.sourcePath : ""
        videoOutput: videoOutput
        loops: MediaPlayer.Infinite
        Component.onCompleted: {
            playTimer.start()
        }
        onSourceChanged: {
            playTimer.start()
        }
    }

    D.BusyIndicator {
        id: playLoader
        anchors.centerIn: parent
        property bool enable: root.isVideo && (mediaplayer.mediaStatus === MediaPlayer.LoadingMedia || playTimer.running)
        running: enable
        opacity: enable ? 1 : 0
        implicitWidth: 24
        implicitHeight: 24
    }

    Timer {
        id: playTimer
        interval: 300
        onTriggered: {
            mediaplayer.play()
        }
    }

    VideoOutput {
        id: videoOutput
        fillMode: VideoOutput.PreserveAspectCrop
        anchors.fill: parent
        antialiasing: true
    }
}
