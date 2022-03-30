/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "namespace.h"
#include <QIcon>
#include <QDebug>

namespace DCC_NAMESPACE
{

/**
 * @brief ModuleData作为ModuleObject的元数据，控制中心以该数据作为主要识别手段
 */
class ModuleData : public QObject
{
    Q_OBJECT
public:
    QString                 Name;           // 名称，作为每个模块的唯一标识，不可为空
    QString                 DisplayName;    // 显示名称，如菜单的名称，页面的标题等，为空则不显示
    QString                 Description;    // 描述，如主菜单的描述信息
    QStringList             ContentText;    // 上下文数据，参与搜索，只可用于终结点：DisplayName -> ContentText(one of it)
    QIcon                   Icon;           // 图标，如主菜单的图标
    int                     Badge;          // 主菜单中的角标, 默认为0不显示，大于0显示
    ModuleData(QObject *parent = nullptr);
    ModuleData(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);
    ModuleData(const QString &name, const QStringList &contentText, QObject *parent = nullptr);
    ModuleData(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent = nullptr);
    ModuleData(const QString &name, const QString &displayName, const QIcon &icon, QObject *parent = nullptr);
    ModuleData(const QString &name, const QString &displayName, const QString &description, const QIcon &icon = QIcon(), QObject *parent = nullptr);
    ModuleData(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QIcon &icon = QIcon(), QObject *parent = nullptr);
    ~ModuleData() {}
    ModuleData(const ModuleData &m);

    bool operator==(const ModuleData &other) const;
};

QDebug inline operator<<(QDebug d, const ModuleData &data)
{
    d.nospace() << QString("ModuleData(Name:%1, DisplayName:%2, Description:%3, ContentText:%4, Icon:%5)")
        .arg(data.Name).arg(data.DisplayName).arg(data.Description).arg(data.ContentText.join('|')).arg(data.Icon.name());
    return d;
}

} // namespace DCC_NAMESAPCE

