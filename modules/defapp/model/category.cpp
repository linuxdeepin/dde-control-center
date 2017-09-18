/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "category.h"
#include <QDebug>
using namespace dcc;
using namespace dcc::defapp;
Category::Category(QObject *parent)
    : QObject(parent)
{

}

void Category::setuserList(const QList<QJsonObject> &list)
{
    if (m_userlist != list && list.size() != 0) {
        m_userlist = list;
        emit userItemChanged(m_userlist);
    }
}

void Category::setappList(const QList<QJsonObject> &list)
{
    if (m_applist != list && list.size() != 0) {
        m_applist = list;
        emit itemsChanged(list);
    }
}

void Category::setDefault(const QJsonObject &def)
{
    if (m_default != def && def.size() != 0) {
        m_default = def;
        emit defaultChanged(def);
    }
}

void Category::setCategory(const QString &category)
{
    if (m_category == category)
        return;

    m_category = category;
    emit categoryNameChanged(category);
}

void Category::addUserItem(const QJsonObject &value)
{
    if(!m_userlist.contains(value)) {
        m_userlist.insert(m_userlist.end(), value);
        emit AdduserItem(value);
    }
}

void Category::delUserItem(const QJsonObject &value)
{
    m_userlist.removeOne(value);
}

