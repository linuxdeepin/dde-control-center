// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

/**
 * @brief 磁盘管理 —— 主页（MainObj 阶段自动加载）
 *
 * 【插件加载流程 - MainObj 阶段】
 * 框架在完成 MetaData→Module→Data 三个阶段后，进入 MainObj 阶段，
 * 自动查找并加载 {PluginName}Main.qml（即此文件 DiskmanagerMain.qml）。
 *
 * 此文件的根 DccObject 通过 parentName: "diskmanager"
 * 挂载到 Diskmanager.qml 根节点下，成为侧边栏项下的子页面。
 *
 * 【dccData 的使用】
 * dccData 全局变量即 C++ DiskManager 对象实例:
 *   dccData.diskInfoList  → Q_PROPERTY，返回磁盘列表
 *   dccData.mountInfoList → Q_PROPERTY，返回挂载点列表
 *   dccData.refresh()     → Q_INVOKABLE 槽，刷新数据
 */
DccObject {
    id: root

    // ============================================================
    // 子页面1: 磁盘概况
    // ============================================================
    DccObject {
        name: "overview"
        parentName: "diskmanager"
        displayName: qsTr("Disk Overview")
        description: qsTr("View information about physical disks installed in your system")
        icon: "drive-harddisk"
        weight: 10
        page: DccRightView {
            spacing: 0

            // ── 顶部概览卡片 ──
            Rectangle {
                id: overviewHeader
                height: overviewHeaderLayout.height + 40
                anchors.left: parent.left
                anchors.right: parent.right
                color: Qt.rgba(0.2, 0.5, 0.9, 0.06)
                radius: 8

                ColumnLayout {
                    id: overviewHeaderLayout
                    anchors.centerIn: parent
                    spacing: 8

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 32

                        // 磁盘总数
                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: dccData.diskInfoList.length
                                font.pixelSize: 32
                                font.bold: true
                                color: D.DTK.platformTheme?.palette?.highlight?.color ?? "#0081FF"
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("Total Disks")
                                font.pixelSize: 12
                                opacity: 0.6
                            }
                        }

                        // SSD 数量
                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: {
                                    var count = 0;
                                    for (var i = 0; i < dccData.diskInfoList.length; i++) {
                                        if (dccData.diskInfoList[i].type === "SSD") count++;
                                    }
                                    return count;
                                }
                                font.pixelSize: 28
                                font.bold: true
                                color: "#27AE60"
                                Behavior on text { NumberAnimation { duration: 300 } }
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("SSD")
                                font.pixelSize: 12
                                opacity: 0.6
                            }
                        }

                        // HDD 数量
                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: {
                                    var count = 0;
                                    for (var i = 0; i < dccData.diskInfoList.length; i++) {
                                        if (dccData.diskInfoList[i].type === "HDD") count++;
                                    }
                                    return count;
                                }
                                font.pixelSize: 28
                                font.bold: true
                                color: "#F39C12"
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("HDD")
                                font.pixelSize: 12
                                opacity: 0.6
                            }
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Click a disk card below to view detailed information")
                        font.pixelSize: 11
                        opacity: 0.45
                    }
                }
            }

            // ── 刷新按钮（带防抖） ──
            Rectangle {
                height: 40
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                color: "transparent"

                // 防抖：1秒内只允许刷新一次
                property bool refreshing: false
                Timer {
                    id: refreshDebounce
                    interval: 1000
                    onTriggered: parent.refreshing = false
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.vertical
                    spacing: 6

                    D.IconLabel {
                        id: refreshIcon
                        icon.name: "view-refresh"
                        icon.width: 16
                        icon.height: 16
                    }
                    Label {
                        text: qsTr("Refresh")
                        font.pixelSize: 12
                        opacity: parent.refreshing ? 0.4 : 0.7
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    enabled: !root.refreshing
                    onClicked: {
                        if (parent.refreshing) return;
                        parent.refreshing = true;
                        refreshDebounce.start();
                        refreshAnim.restart();
                        dccData.refresh();
                    }
                }

                RotationAnimator {
                    id: refreshAnim
                    target: refreshIcon
                    from: 0
                    to: 360
                    duration: 600
                    easing.type: Easing.OutCubic
                }
            }

            // ── 磁盘列表 ──
            DccGroupView {
                isGroup: false
                DccObject {
                    name: "diskList"
                    parentName: "diskmanager/overview"
                    displayName: qsTr("Physical Disks")
                    weight: 10
                    backgroundType: DccObject.Normal
                    pageType: DccObject.Group
                    DccRepeater {
                        model: dccData.diskInfoList
                        delegate: DccObject {
                            name: "disk_" + modelData.name
                            parentName: "diskmanager/overview/diskList"
                            displayName: modelData.name + " (" + modelData.size + ")"
                            icon: modelData.type === "SSD" ? "drive-solidstate" : "drive-harddisk"
                            description: modelData.model ? modelData.model : qsTr("Unknown Model")
                            weight: (model.index + 1) * 10
                            backgroundType: DccObject.Normal
                            pageType: DccObject.Editor
                            page: Item {
                                id: diskCardRoot
                                implicitHeight: diskCardLayout.height + 24

                                // 鼠标悬停效果
                                property bool hovered: false
                                // 磁盘设备名（供内层 Repeater 引用）
                                readonly property string diskName: modelData.name

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 8
                                    color: parent.hovered ? Qt.rgba(0.2, 0.5, 0.9, 0.04) : "transparent"
                                    Behavior on color { ColorAnimation { duration: 200 } }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: parent.hovered = true
                                    onExited: parent.hovered = false
                                }

                                ColumnLayout {
                                    id: diskCardLayout
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.vertical
                                    anchors.margins: 8
                                    spacing: 6

                                    // ── 第一行: 设备名 + 类型标签 + 总大小 ──
                                    RowLayout {
                                        spacing: 8
                                        Label {
                                            text: "/dev/" + modelData.name
                                            font.bold: true
                                            font.pixelSize: 14
                                        }
                                        // 类型标签
                                        Rectangle {
                                            radius: 4
                                            color: modelData.type === "SSD" ? "#27AE60"
                                                 : modelData.type === "HDD" ? "#F39C12"
                                                 : "#95A5A6"
                                            implicitWidth: typeLabel.implicitWidth + 12
                                            implicitHeight: 20
                                            Label {
                                                id: typeLabel
                                                anchors.centerIn: parent
                                                text: modelData.type
                                                color: "white"
                                                font.pixelSize: 11
                                                font.bold: true
                                            }
                                        }
                                        Item { Layout.fillWidth: true }
                                        // 物理总容量
                                        Label {
                                            text: qsTr("Total: %1").arg(modelData.size)
                                            font.pixelSize: 12
                                            opacity: 0.65
                                        }
                                    }

                                    // ── 第二行: 型号 ──
                                    Label {
                                        text: modelData.model ? modelData.model : qsTr("Model information unavailable")
                                        font.pixelSize: 12
                                        opacity: 0.5
                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                    }

                                    // ── 第三行: 已用/可用/总用量进度条 ──
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 12
                                        radius: 6
                                        color: Qt.rgba(0, 0, 0, 0.06)

                                        // 已用空间条
                                        Rectangle {
                                            id: usedBar
                                            radius: 6
                                            height: parent.height
                                            width: modelData.hasUsageData
                                                   ? parent.width * Math.min(safePercent(modelData.usagePercent) / 100.0, 1.0)
                                                   : 0

                                            gradient: Gradient {
                                                orientation: Gradient.Horizontal
                                                GradientStop {
                                                    position: 0.0
                                                    color: safePercent(modelData.usagePercent) > 90 ? "#E74C3C"
                                                         : safePercent(modelData.usagePercent) > 75 ? "#F39C12"
                                                         : modelData.hasUsageData ? "#3498DB" : "transparent"
                                                }
                                                GradientStop {
                                                    position: 1.0
                                                    color: safePercent(modelData.usagePercent) > 90 ? "#C0392B"
                                                         : safePercent(modelData.usagePercent) > 75 ? "#E67E22"
                                                         : modelData.hasUsageData ? "#2980B9" : "transparent"
                                                }
                                            }

                                            Behavior on width { NumberAnimation { duration: 800; easing.type: Easing.OutCubic } }
                                        }

                                        // 无数据时的占位提示
                                        Label {
                                            anchors.centerIn: parent
                                            text: qsTr("No mounted partitions")
                                            font.pixelSize: 10
                                            opacity: modelData.hasUsageData ? 0 : 0.35
                                            visible: !modelData.hasUsageData
                                        }
                                    }

                                    // ── 第四行: 用量明细（已用 / 可用 / 使用率）──
                                    RowLayout {
                                        spacing: 12
                                        // 已用空间（红色强调）
                                        RowLayout {
                                            spacing: 3
                                            Rectangle {
                                                width: 8; height: 8; radius: 4
                                                color: safePercent(modelData.usagePercent) > 90 ? "#E74C3C"
                                                     : safePercent(modelData.usagePercent) > 75 ? "#F39C12"
                                                     : modelData.hasUsageData ? "#3498DB" : "transparent"
                                                visible: modelData.hasUsageData
                                            }
                                            Label {
                                                text: qsTr("Used: %1").arg(modelData.hasUsageData ? modelData.usedFormatted : "--")
                                                font.pixelSize: 12
                                                opacity: 0.75
                                                color: safePercent(modelData.usagePercent) > 90 ? "#E74C3C" : "inherit"
                                            }
                                        }

                                        // 可用空间（绿色）
                                        RowLayout {
                                            spacing: 3
                                            Rectangle {
                                                width: 8; height: 8; radius: 4
                                                color: "#27AE60"
                                                visible: modelData.hasUsageData
                                            }
                                            Label {
                                                text: qsTr("Free: %1").arg(modelData.hasUsageData ? modelData.availableFormatted : "--")
                                                font.pixelSize: 12
                                                opacity: 0.75
                                                color: "#27AE60"
                                            }
                                        }

                                        Item { Layout.fillWidth: true }

                                        // 使用率百分比
                                        Label {
                                            text: modelData.hasUsageData
                                                  ? qsTr("%1% used").arg(safePercent(modelData.usagePercent))
                                                  : qsTr("--")
                                            font.pixelSize: 12
                                            font.bold: safePercent(modelData.usagePercent) > 75
                                            opacity: 0.8
                                            color: safePercent(modelData.usagePercent) > 90 ? "#E74C3C"
                                                 : safePercent(modelData.usagePercent) > 75 ? "#F39C12"
                                                 : "inherit"

                                            // 高使用率时闪烁
                                            NumberAnimation on opacity {
                                                running: safePercent(modelData.usagePercent) > 90
                                                from: 0.4; to: 1.0
                                                duration: 800; loops: Animation.Infinite
                                            }
                                        }
                                    }

                                    // ── 分割线（轻量化） ──
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 1
                                        color: Qt.rgba(0, 0, 0, 0.06)
                                    }

                                    // ── 第五行: 分区的用量详情条（微缩条状图） ──
                                    Flow {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Repeater {
                                            id: diskMountsRepeater
                                            model: dccData.mountInfoList

                                            // 外层磁盘名（避免内层 Repeater 中 modelData 被覆盖）
                                            property string outerDiskName: diskCardRoot.diskName

                                            delegate: Rectangle {
                                                id: miniBarRoot
                                                // 通过 device 判断是否属于本磁盘
                                                readonly property string mountDevice: {
                                                    var dev = modelData.device;
                                                    if (dev.startsWith("/dev/")) dev = dev.substring(5);
                                                    return dev;
                                                }
                                                visible: mountDevice.length > 0
                                                         && mountDevice.startsWith(diskMountsRepeater.outerDiskName)

                                                width: 20
                                                height: 20
                                                radius: 4
                                                color: {
                                                    var pct = safePercent(modelData.usagePercent);
                                                    return pct > 90 ? Qt.rgba(0.91, 0.30, 0.24, 0.7)
                                                         : pct > 75 ? Qt.rgba(0.95, 0.61, 0.07, 0.7)
                                                         : Qt.rgba(0.20, 0.60, 0.86, 0.5);
                                                }

                                                // 迷你进度条
                                                Rectangle {
                                                    anchors.bottom: parent.bottom
                                                    anchors.left: parent.left
                                                    anchors.right: parent.right
                                                    height: parent.height * Math.min(safePercent(modelData.usagePercent) / 100.0, 1.0)
                                                    radius: 4
                                                    color: Qt.rgba(0, 0, 0, 0.15)
                                                }

                                                // 鼠标悬停显示分区名
                                                ToolTip {
                                                    visible: parentMouseArea.containsMouse
                                                    text: modelData.mountPoint + "\n"
                                                          + qsTr("Used: %1 / %2 (%3%)")
                                                              .arg(modelData.used).arg(modelData.total).arg(safePercent(modelData.usagePercent))
                                                    delay: 300
                                                }
                                                MouseArea {
                                                    id: parentMouseArea
                                                    anchors.fill: parent
                                                    hoverEnabled: true
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ============================================================
    // 子页面2: 挂载点信息
    // ============================================================
    DccObject {
        name: "mounts"
        parentName: "diskmanager"
        displayName: qsTr("Mount Points")
        description: qsTr("View mounted partitions, usage, and available space")
        icon: "drive-removable-media"
        weight: 20
        page: DccRightView {
            spacing: 0

            // ── 挂载点概览卡片 ──
            Rectangle {
                height: mountSummaryLayout.height + 40
                anchors.left: parent.left
                anchors.right: parent.right
                color: Qt.rgba(0.2, 0.8, 0.4, 0.06)
                radius: 8

                ColumnLayout {
                    id: mountSummaryLayout
                    anchors.centerIn: parent
                    spacing: 8

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 28

                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: dccData.mountInfoList.length
                                font.pixelSize: 32
                                font.bold: true
                                color: D.DTK.platformTheme?.palette?.highlight?.color ?? "#0081FF"
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("Partitions")
                                font.pixelSize: 12
                                opacity: 0.6
                            }
                        }

                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: {
                                    var total = 0;
                                    for (var i = 0; i < dccData.mountInfoList.length; i++) {
                                        total += dccData.mountInfoList[i].totalBytes;
                                    }
                                    return total > 0 ? formatAutoSize(total) : "N/A";
                                }
                                font.pixelSize: 18
                                font.bold: true
                                opacity: 0.8
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("Total Space")
                                font.pixelSize: 12
                                opacity: 0.6
                            }
                        }

                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: {
                                    var total = 0;
                                    for (var i = 0; i < dccData.mountInfoList.length; i++) {
                                        total += dccData.mountInfoList[i].usedBytes;
                                    }
                                    return total > 0 ? formatAutoSize(total) : "N/A";
                                }
                                font.pixelSize: 18
                                font.bold: true
                                color: "#E74C3C"
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("Used")
                                font.pixelSize: 12
                                opacity: 0.6
                            }
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Usage bar color: \t\tGreen = Safe  \t\tYellow = Warning  \t\tRed = Critical")
                        font.pixelSize: 10
                        opacity: 0.35
                    }
                }
            }

            // ── 挂载点列表 ──
            DccGroupView {
                isGroup: false
                DccObject {
                    name: "mountList"
                    parentName: "diskmanager/mounts"
                    displayName: qsTr("Partitions && Mount Points")
                    weight: 10
                    backgroundType: DccObject.Normal
                    pageType: DccObject.Group
                    DccRepeater {
                        model: dccData.mountInfoList
                        delegate: DccObject {
                            name: "mount_" + modelData.device.replace("/", "_")
                            parentName: "diskmanager/mounts/mountList"
                            displayName: modelData.mountPoint + "  [" + modelData.device + "]"
                            description: qsTr("%1 filesystem · %2 available").arg(modelData.filesystem).arg(modelData.available)
                            weight: (model.index + 1) * 10
                            backgroundType: DccObject.Normal
                            pageType: DccObject.Editor
                            page: Item {
                                implicitHeight: mountCardLayout.height + 20

                                ColumnLayout {
                                    id: mountCardLayout
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.vertical
                                    anchors.margins: 8
                                    spacing: 8

                                    // 第一行: 挂载点 + 设备
                                    RowLayout {
                                        spacing: 8
                                        Label {
                                            text: modelData.mountPoint
                                            font.bold: true
                                            font.pixelSize: 14
                                        }
                                        Label {
                                            text: modelData.device
                                            font.pixelSize: 12
                                            opacity: 0.55
                                        }
                                        Item { Layout.fillWidth: true }
                                        // 文件系统标签
                                        Rectangle {
                                            radius: 3
                                            color: Qt.rgba(0.5, 0.5, 0.5, 0.12)
                                            implicitWidth: fsLabel.implicitWidth + 10
                                            implicitHeight: 18
                                            Label {
                                                id: fsLabel
                                                anchors.centerIn: parent
                                                text: modelData.filesystem
                                                font.pixelSize: 10
                                                opacity: 0.7
                                            }
                                        }
                                    }

                                    // 第二行: 使用率进度条（带动画）
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 10
                                        radius: 5
                                        color: Qt.rgba(0, 0, 0, 0.06)

                                        readonly property int safeUsage: safePercent(modelData.usagePercent)

                                        Rectangle {
                                            id: usageBar
                                            width: parent.width * Math.min(parent.safeUsage / 100.0, 1.0)
                                            height: parent.height
                                            radius: 5

                                            // 渐变色: 根据使用率变化
                                            gradient: Gradient {
                                                orientation: Gradient.Horizontal
                                                GradientStop {
                                                    position: 0.0
                                                    color: parent.parent.safeUsage > 90 ? "#E74C3C"
                                                         : parent.parent.safeUsage > 75 ? "#F39C12"
                                                         : "#27AE60"
                                                }
                                                GradientStop {
                                                    position: 1.0
                                                    color: parent.parent.safeUsage > 90 ? "#C0392B"
                                                         : parent.parent.safeUsage > 75 ? "#E67E22"
                                                         : "#2ECC71"
                                                }
                                            }

                                            Behavior on width { NumberAnimation { duration: 800; easing.type: Easing.OutCubic } }

                                            // 脉冲动画（使用率高时闪烁提醒）
                                            SequentialAnimation {
                                                running: parent.parent.safeUsage > 90
                                                loops: Animation.Infinite
                                                PropertyAnimation {
                                                    target: usageBar
                                                    property: "opacity"
                                                    to: 0.7
                                                    duration: 800
                                                }
                                                PropertyAnimation {
                                                    target: usageBar
                                                    property: "opacity"
                                                    to: 1.0
                                                    duration: 800
                                                }
                                            }
                                        }

                                        // 百分比文字
                                        Label {
                                            anchors.right: parent.right
                                            anchors.rightMargin: 4
                                            anchors.verticalCenter: parent.vertical
                                            text: parent.parent.safeUsage + "%"
                                            font.pixelSize: 10
                                            font.bold: true
                                            color: parent.parent.safeUsage > 90 ? "#E74C3C"
                                                 : parent.parent.safeUsage > 75 ? "#F39C12"
                                                 : "#27AE60"
                                        }
                                    }

                                    // 第三行: 用量详情
                                    RowLayout {
                                        spacing: 16
                                        Label {
                                            text: qsTr("Used: %1").arg(modelData.used)
                                            font.pixelSize: 12
                                            opacity: 0.7
                                        }
                                        Label {
                                            text: qsTr("Total: %1").arg(modelData.total)
                                            font.pixelSize: 12
                                            opacity: 0.7
                                        }
                                        Label {
                                            text: qsTr("Avail: %1").arg(modelData.available)
                                            font.pixelSize: 12
                                            opacity: modelData.usagePercent > 90 ? 1.0 : 0.7
                                            color: modelData.usagePercent > 90 ? "#E74C3C" : "inherit"
                                            font.bold: modelData.usagePercent > 90
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ============================================================
    // 工具函数: 自动格式化字节大小
    // ============================================================
    function formatAutoSize(bytes) {
        if (bytes <= 0) return "0 B";
        var units = ["B", "KB", "MB", "GB", "TB"];
        var unitIndex = 0;
        var size = bytes;
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        return size.toFixed(1) + " " + units[unitIndex];
    }

    // ============================================================
    // 工具函数: 安全获取使用率（防止 undefined/NaN 导致 QML 警告）
    // ============================================================
    function safePercent(value) {
        if (value === undefined || value === null) return 0;
        var num = Number(value);
        return isNaN(num) ? 0 : Math.max(0, Math.min(100, num));
    }
}