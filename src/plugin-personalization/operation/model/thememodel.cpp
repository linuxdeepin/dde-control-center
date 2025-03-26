//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "thememodel.h"

ThemeModel::ThemeModel(QObject *parent)
    : QObject(parent)
{
}

QStringList ThemeModel::keys()
{
    return m_keys;
}

void ThemeModel::addItem(const QString &id, const QJsonObject &json)
{
    if (m_list.contains(id)) {
        m_keys.removeOne(id);
        m_keys.append(id);
        return;
    }
    m_keys.append(id);
    m_list.insert(id, json);
    Q_EMIT itemAdded(json);
}

void ThemeModel::setDefault(const QString &value)
{
    m_default = value;
    Q_EMIT defaultChanged(value);
    for (const auto &themeItem : m_list) {
        if (themeItem.value("Id") == value) {
            const QString &themeName = themeItem.value("Name").toString();
            m_currentThemeName = themeName;
            Q_EMIT currentThemeNameChanged(themeName);
            break;
        }
    }
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
    m_keys.removeOne(id);
    Q_EMIT itemRemoved(id);
}
