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

#include "category.h"
#include <QDebug>
using namespace dcc;
using namespace dcc::defapp;
Category::Category(QObject *parent)
    : QObject(parent)
{

}

void Category::setDefault(const App &def)
{
    if (m_default.Id != def.Id) {
        m_default = def;
        Q_EMIT defaultChanged(def);
    }
}

void Category::setCategory(const QString &category)
{
    if (m_category == category)
        return;

    m_category = category;
    Q_EMIT categoryNameChanged(category);
}

void Category::addUserItem(const App &value)
{
    if (value.isUser) {
        if (m_userAppList.contains(value)) return;
        m_userAppList << value;
    }
    else {
        if (m_systemAppList.contains(value)) return;
        m_systemAppList << value;
    }

    m_applist << value;
    Q_EMIT addedUserItem(value);
}

void Category::delUserItem(const App &value)
{
    bool isRemove = false;

    if (value.isUser) {
       isRemove = m_userAppList.removeOne(value);
    }
    else {
        isRemove = m_systemAppList.removeOne(value);
    }

    if (isRemove) {
        m_applist.removeOne(value);
        Q_EMIT removedUserItem(value);
    }
}
