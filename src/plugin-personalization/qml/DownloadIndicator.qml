// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import org.deepin.dtk 1.0 as D

Canvas {
    property real progress: 0
    onProgressChanged: requestPaint()
    onVisibleChanged: if (visible) requestPaint()
    Component.onCompleted: requestPaint()

    Behavior on progress {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 400
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 400
        }
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 400
        }
    }

    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();

        var cx = width / 2;
        var cy = height / 2;

        var borderWidth = 1;
        var ringWidth = 2;

        var outerRadius = width / 2 - borderWidth / 2;
        var ringRadius = outerRadius - 1;

        var borderColor = Qt.rgba(0, 0, 0, 0.08);
        var trackColor = Qt.rgba(250, 250, 250, 0.8);

        // 外边框
        ctx.beginPath();
        ctx.arc(cx, cy, outerRadius, 0, Math.PI * 2);
        ctx.lineWidth = borderWidth;
        ctx.strokeStyle = borderColor;
        ctx.stroke();

        // 内边框
        ctx.beginPath();
        ctx.arc(cx, cy, ringRadius - ringWidth / 2, 0, Math.PI * 2);
        ctx.lineWidth = borderWidth;
        ctx.strokeStyle = borderColor;
        ctx.stroke();

        // 背景轨道
        ctx.beginPath();
        ctx.arc(cx, cy, ringRadius, 0, Math.PI * 2);
        ctx.lineWidth = ringWidth;
        ctx.strokeStyle = trackColor;
        ctx.stroke();

        // 进度条
        var startAngle = -Math.PI / 2;
        var endAngle = startAngle + Math.PI * 2 * progress;

        ctx.beginPath();
        ctx.arc(cx, cy, ringRadius, startAngle, endAngle);

        ctx.lineWidth = ringWidth;
        ctx.lineCap = "round";
        ctx.strokeStyle = D.DTK.platformTheme.activeColor;

        ctx.stroke();
    }
}