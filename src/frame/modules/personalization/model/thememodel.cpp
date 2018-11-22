/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "thememodel.h"

using namespace dcc;
using namespace dcc::personalization;

ThemeModel::ThemeModel(QObject * parent) : QObject(parent)
{
}

void ThemeModel::addItem(const QString &id, const QJsonObject &json)
{
    if (m_list.contains(id))
        return;

    m_list.insert(id, json);
    Q_EMIT itemAdded(json);
}

void ThemeModel::setDefault(const QString &value)
{
    m_default = value;
    Q_EMIT defaultChanged(value);
}

QMap<QString, QString> ThemeModel::getPicList() const
{
    return m_picList;
}

void ThemeModel::addPic(const QString &id, const QString &picPath)
{
    m_picList.insert(id, picPath);
    Q_EMIT picAdded(id, picPath);
}

void ThemeModel::removeItem(const QString &id)
{
    m_list.remove(id);
    Q_EMIT itemRemoved(id);
}
