// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
