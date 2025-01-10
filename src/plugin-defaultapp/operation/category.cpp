// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "category.h"
#include <QDebug>

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

void Category::clear()
{
    bool clearFlag = !m_applist.isEmpty();

    m_systemAppList.clear();
    m_userAppList.clear();
    m_applist.clear();
    if (clearFlag)
        Q_EMIT clearAll();
}

void Category::addUserItem(const App &value)
{
    if (value.isUser) {
        if (m_userAppList.contains(value)) return;
        m_userAppList << value;
    } else {
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
    } else {
        isRemove = m_systemAppList.removeOne(value);
    }

    if (isRemove) {
        m_applist.removeOne(value);
        Q_EMIT removedUserItem(value);
    }
}
