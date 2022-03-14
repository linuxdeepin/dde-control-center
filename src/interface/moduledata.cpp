/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "interface/moduledata.h"

DCC_USE_NAMESPACE

ModuleData::ModuleData(QObject *parent)
    : QObject(parent) {}

ModuleData::ModuleData(const QString &name, const QString &displayName, QObject *parent)
    : ModuleData(name, displayName, QStringList(), parent)
{

}

ModuleData::ModuleData(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleData(name, {}, contentText, parent)
{

}

ModuleData::ModuleData(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleData(name, displayName, {}, contentText, QIcon(), parent)
{

}

ModuleData::ModuleData(const QString &name, const QString &displayName, const QIcon &icon, QObject *parent)
    : ModuleData(name, displayName, {}, icon, parent)
{

}

ModuleData::ModuleData(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleData(name, displayName, description, {}, icon, parent)
{

}

ModuleData::ModuleData(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QIcon &icon, QObject *parent)
    : QObject(parent)
    , Name(name)
    , DisplayName(displayName)
    , Description(description)
    , ContentText(contentText)
    , Icon(icon)
    , Badge(0)
{
    if (icon.isNull())
        Icon = QIcon::fromTheme(QString("dcc_%1").arg(name));
}

ModuleData::ModuleData(const ModuleData &m) {
    Name = m.Name;
    DisplayName = m.DisplayName;
    Description = m.Description;
    ContentText = m.ContentText;
    Icon = m.Icon;
}

bool ModuleData::operator==(const ModuleData &other) const
{
    return Name == other.Name && DisplayName == other.DisplayName && Description == other.Description
        && ContentText == other.ContentText && Icon.name() == other.Icon.name();
}

