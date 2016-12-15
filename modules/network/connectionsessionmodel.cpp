#include "connectionsessionmodel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

using namespace dcc::network;

ConnectionSessionModel::ConnectionSessionModel(QObject *parent)
    : QObject(parent)
{

}

void ConnectionSessionModel::setSections(const QList<QString> &sections)
{
    if (sections != m_sections)
    {
        m_sections = sections;

        emit sectionsChanged(m_sections);
    }
}

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

            m_keys[keySection][keyKey] = key;
        }
    }

    emit keysChanged(m_keys);
}
