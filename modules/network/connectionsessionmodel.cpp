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

#include "connectionsessionmodel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

using namespace dcc::network;

ConnectionSessionModel::ConnectionSessionModel(QObject *parent)
    : QObject(parent)
{

}

const QJsonObject ConnectionSessionModel::keysInfo(const QString &section, const QString &vKey) const
{
    for (const auto &item : m_visibleItems.value(section))
    {
        if (item.value("Key").toString() == vKey)
            return item;
    }

    return QJsonObject();
}

void ConnectionSessionModel::setDeletable(const bool deletable)
{
    if (m_deletable != deletable)
    {
        m_deletable = deletable;

        emit deletableChanged(m_deletable);
    }
}

void ConnectionSessionModel::setType(const QString &type)
{
    if (m_type != type)
    {
        m_type = type;

        emit typeChanged(m_type);
    }
}

void ConnectionSessionModel::setConnUuid(const QString &connUuid)
{
    if (m_uuid != connUuid)
    {
        m_uuid = connUuid;

        emit uuidChanged(m_uuid);
    }
}

void ConnectionSessionModel::setErrors(const NetworkErrors &errors)
{
    m_errors = errors;

    emit errorsChanged(m_errors);
}

void ConnectionSessionModel::setAvailableItems(const QString &items)
{
    m_sections.clear();
    m_visibleItems.clear();

    const QJsonArray sections = QJsonDocument::fromJson(items.toUtf8()).array();
    for (const auto &sectionObject : sections)
    {
        const auto section = sectionObject.toObject();
        const auto vsName = section.value("VirtualSection").toString();
        const auto keys = section.value("Keys").toArray();

        m_sections.append(vsName);
        for (const auto &keyObject : keys)
            m_visibleItems[vsName].append(keyObject.toObject());
    }

    emit visibleItemsChanged(m_visibleItems);
}

//void ConnectionSessionModel::setVisibleKeys(const QMap<QString, QStringList> &keys)
//{
//    if (m_visibleKeys != keys)
//    {
//        m_visibleKeys = keys;

//        emit visibleItemsChanged(m_visibleKeys);
//    }
//}

void ConnectionSessionModel::setAllKeys(const QString &allKeys)
{
    const auto sectionArray = QJsonDocument::fromJson(allKeys.toUtf8()).array();
    for (const auto &sectionObect : sectionArray)
    {
        const auto section = sectionObect.toObject();
        const auto sectionKey = section.value("VirtualSection").toString();
        const auto sectionName = section.value("Name").toString();

        m_sectionName[sectionKey] = sectionName;

        const auto keyList = section.value("Keys").toArray();
        for (const auto &keyObject : keyList)
        {
            const auto key = keyObject.toObject();
            const auto keySection = key.value("Section").toString();
            const auto keyKey = key.value("Key").toString();

            m_vks[sectionKey][keyKey] = key;
            m_virtualSectionName[keySection] = sectionKey;
        }
    }

    emit keysChanged(m_vks);
}
