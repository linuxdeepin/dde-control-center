// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOCK_CONSTANTS_H
#define DOCK_CONSTANTS_H

#include <QByteArray>
#include <QSize>
#include <QColor>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(DOCK_APP)

namespace Dock {

enum ItemType {
    ItemType_Launcher,
    ItemType_App,
    ItemType_Plugins,
    ItemType_FixedPlugin,
    ItemType_Placeholder,
    ItemType_Overflow,
    ItemType_TrayPlugin,
    ItemType_ToolPlugin
};

const QByteArray REQUEST_DOCK_KEY = "RequestDock";
const QByteArray RequestDockKeyFallback = "text/plain";
const QByteArray DesktopMimeType = "application/x-desktop";
const QByteArray QuickPluginMimeType = "plugin/quick";
const QByteArray TRAY_PLUGIN_MIME_TYPE = "plugin/tray";

const QString TRAY_PLUGIN_NAME = "tray";
const QColor ITEM_BACKGROUND_BRUSH(0, 0, 0, 0.05 * 255);    // 黑色，百分之五的透明度

// 飞行模式在之前控制中心-网络-飞行模式中处理“是否在任务栏显示”功能时错误使用了 module中的enable 属性，删除“是否在任务栏显示”功能时有兼容行问题
// 之前设置过禁用过的用户在快捷面板和任务栏中都不会出现飞行模式
// 在此处添加白名单，废弃掉飞行模式的 enable 属性，固定返回 true。
const QStringList MODULE_ENABLE_WHITE_LIST = {"airplane-mode"};

}

#endif // DOCK_CONSTANTS_H
