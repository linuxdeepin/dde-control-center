// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dockpluginsortproxymodel.h"

#include <QCollator>
#include <QLocale>

DockPluginSortProxyModel::DockPluginSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true); // 启用动态排序，当源模型数据变化时自动重新排序
    setSortRole(Qt::DisplayRole); // 默认按 displayName 排序
    sort(0, Qt::AscendingOrder);
}

DockPluginSortProxyModel::StringGroup DockPluginSortProxyModel::classifyString(const QString &s) const
{
    for (QChar c : s) {
        if (c.isSpace())
            continue;

        if (c.isDigit())
            return StringGroup::Digit;

        ushort u = c.unicode();
        if ((u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z'))
            return StringGroup::Latin;

        return StringGroup::CJK;
    }
    return StringGroup::CJK;
}

// 按显示名，支持“数字 -> 英文 -> 中文”三段排序
bool DockPluginSortProxyModel::lessThan(const QModelIndex &leftIndex, const QModelIndex &rightIndex) const
{
    const QString left = sourceModel()->data(leftIndex, Qt::DisplayRole).toString();
    const QString right = sourceModel()->data(rightIndex, Qt::DisplayRole).toString();

    const StringGroup leftGroup  = classifyString(left);
    const StringGroup rightGroup = classifyString(right);

    // 不同组：直接比优先级
    if (leftGroup != rightGroup)
        return static_cast<int>(leftGroup) < static_cast<int>(rightGroup);

    // 数字：自然数字排序
    if (leftGroup == StringGroup::Digit) {
        QCollator digitCollator(QLocale::c());
        digitCollator.setNumericMode(true);
        digitCollator.setCaseSensitivity(Qt::CaseInsensitive);
        return digitCollator.compare(left, right) < 0;
    }

    // 英文：字母序
    if (leftGroup == StringGroup::Latin) {
        return QString::compare(left, right, Qt::CaseInsensitive) < 0;
    }

    // 中文：拼音序（强制中文 locale）
    QCollator zhCollator(QLocale(QLocale::Chinese, QLocale::China));
    zhCollator.setNumericMode(true);
    zhCollator.setCaseSensitivity(Qt::CaseInsensitive);

    return zhCollator.compare(left, right) < 0;
}
