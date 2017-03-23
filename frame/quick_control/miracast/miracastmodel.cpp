#include "miracastmodel.h"

#define LINK_PREFIX "/org/freedesktop/miracle/wifi/link/"
#define PEER_PREFIX "/org/freedesktop/miracle/wifi/peer/"

MiracastModel::MiracastModel(QObject *parent)
    : QObject(parent)
{

}

void MiracastModel::addPeer(const PeerInfo &peer)
{
    emit peerAdded(peer);
}

void MiracastModel::addLink(const LinkInfo &link)
{
    if (m_links.contains(link))
        return;

    m_links.append(link);

    emit linkAdded(link);
}

void MiracastModel::removeLink(const QDBusObjectPath &path)
{
    for (int i = 0; i != m_links.size(); ++i)
    {
        if (m_links[i].m_dbusPath == path)
        {
            m_links.removeAt(i);
            emit linkRemoved(path);

            return;
        }
    }
}

void MiracastModel::setLinks(const QList<LinkInfo> &links)
{
    for (const auto &link : links)
        addLink(link);
}

void MiracastModel::onPathAdded(const QDBusObjectPath &path, const QString &info)
{
    const QJsonObject infoObject = QJsonDocument::fromJson(info.toUtf8()).object();
    const QString objectPath = path.path();

    if (objectPath.startsWith(LINK_PREFIX))
        return addLink(LinkInfo::fromJson(infoObject));
    else if (objectPath.startsWith(PEER_PREFIX))
        return addPeer(PeerInfo::fromJson(infoObject));

    qDebug() << path.path() << info;
}

void MiracastModel::onPathRemoved(const QDBusObjectPath &path)
{
    qDebug() << path.path();

    const QString objectPath = path.path();
    if (objectPath.startsWith(LINK_PREFIX))
        return removeLink(path);
}

void MiracastModel::onMiracastEvent(const uchar type, const QDBusObjectPath &path)
{
    qDebug() << type << path.path();

    switch (type)
    {
    case LinkManaged:
        linkByPath(path).m_managed = true;
        emit requestLinkScanning(path, true);
        break;
    case LinkUnmanaged:
        linkByPath(path).m_managed = false;
        break;
    default:;
    }
}

LinkInfo &MiracastModel::linkByPath(const QDBusObjectPath &path)
{
    for (auto it(m_links.begin()); it != m_links.end(); ++it)
        if (it->m_dbusPath == path)
            return *it;

    Q_UNREACHABLE();
}
