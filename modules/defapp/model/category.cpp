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
    if (m_default != def) {
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

void Category::addUserItem(const App &value)
{
    if (m_applist.contains(value)) return;

    m_applist << value;
    emit addedUserItem(value);
}

void Category::delUserItem(const App &value)
{
    m_applist.removeOne(value);

    emit removedUserItem(value);
}
