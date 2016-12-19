#include "connectionsessionmodel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

using namespace dcc::network;

ConnectionSessionModel::ConnectionSessionModel(QObject *parent)
    : QObject(parent)
{

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
        {
            const auto key = keyObject.toObject();
            const auto vkName = key.value("Key").toString();

            m_visibleItems[vsName].append(vkName);
        }
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
//            const auto keySection = key.value("Section").toString();
            const auto keyKey = key.value("Key").toString();

            m_keys[sectionKey][keyKey] = key;
        }
    }

    emit keysChanged(m_keys);
}
