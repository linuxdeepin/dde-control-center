#include "miracastmodel.h"

#define LINK_PREFIX "/org/freedesktop/miracle/wifi/link/"
#define SINK_PREFIX "/org/freedesktop/miracle/wfd/sink/"

using namespace dcc;
using namespace dcc::display;

MiracastModel::MiracastModel(QObject *parent)
    : QObject(parent)
{

}

MiracastDeviceModel *MiracastModel::deviceModelByPath(const QString &path)
{
    return m_deviceModelList[path];
}
void MiracastModel::addSink(const SinkInfo &peer)
{
    if (m_sinks.contains(peer))
        return;

    m_sinks.append(peer);
    m_deviceModelList[peer.m_linkPath.path()]->onSinkAdded(peer);
}

void MiracastModel::addLink(const LinkInfo &link)
{
    if (m_links.contains(link))
        return;

    m_links.append(link);
    m_deviceModelList.insert(link.m_dbusPath.path(), new MiracastDeviceModel(link));
    emit linkAdded(link);

    if (link.m_managed && !link.m_p2pScanning)
        emit requestLinkScanning(link.m_dbusPath, true);
}

void MiracastModel::removeLink(const QDBusObjectPath &path)
{
    for (int i = 0; i != m_links.size(); ++i)
    {
        if (m_links[i].m_dbusPath == path)
        {
            m_links.removeAt(i);
            m_deviceModelList.value(path.path())->deleteLater();
            m_deviceModelList.remove(path.path());
            emit linkRemoved(path);

            return;
        }
    }

}

void MiracastModel::removeSink(const QString &sinkInfo)
{
    qDebug() << "remove Sink" << sinkInfo;

    const QJsonObject infoObject = QJsonDocument::fromJson(sinkInfo.toUtf8()).object();
    const SinkInfo info = SinkInfo::fromJson(infoObject);

    m_deviceModelList[info.m_linkPath.path()]->onSinkRemoved(info);
    m_sinks.removeOne(info);
}

void MiracastModel::setLinks(const QList<LinkInfo> &links)
{
    for (const auto &link : links)
        addLink(link);
}

void MiracastModel::setSinks(const QList<SinkInfo> &sinks)
{
    for (const auto &sink : sinks)
        addSink(sink);
}

void MiracastModel::onPathAdded(const QDBusObjectPath &path, const QString &info)
{
    const QJsonObject infoObject = QJsonDocument::fromJson(info.toUtf8()).object();
    const QString objectPath = path.path();

    if (objectPath.startsWith(LINK_PREFIX))
        return addLink(LinkInfo::fromJson(infoObject));
    else if (objectPath.startsWith(SINK_PREFIX))
        return addSink(SinkInfo::fromJson(infoObject));

    qDebug() << path.path() << info;
}

void MiracastModel::onPathRemoved(const QDBusObjectPath &path, const QString &info)
{
    const QString objectPath = path.path();
    if (objectPath.startsWith(LINK_PREFIX))
        return removeLink(path);
    else if (objectPath.startsWith(SINK_PREFIX))
        return removeSink(info);

    qDebug() << path.path() << info;
}

void MiracastModel::onMiracastEvent(const uchar type, const QDBusObjectPath &path)
{
    qDebug() << type << path.path();

    switch (type)
    {
    case LinkManaged:
        linkByPath(path).m_managed = true;
        deviceModelByPath(path.path())->onLinkManageChanged(true);
        emit requestLinkScanning(path, true);
        break;
    case LinkUnmanaged:
        linkByPath(path).m_managed = false;
        deviceModelByPath(path.path())->onLinkManageChanged(false);
        break;
    case SinkConnected:
        sinkStateChanged(path, true);
        emit sinkConnected();
        break;
    case SinkConnectFailed:
    case SinkDisconnected:
        sinkStateChanged(path, false);

        scanAllLinks();
        break;
    default:;
    }
}

void MiracastModel::scanAllLinks()
{
    for (const auto &link : m_links)
    {
        if (link.m_managed && !link.m_p2pScanning)
            emit requestLinkScanning(link.m_dbusPath, true);
    }
}

void MiracastModel::sinkStateChanged(const QDBusObjectPath &path, bool state)
{
    for (auto it(m_sinks.begin()); it != m_sinks.end(); ++it) {
        if (it->m_sinkPath == path) {
            deviceModelByPath(it->m_linkPath.path())->onSinkConnect(path, state);
            return;
        }
    }
}

LinkInfo &MiracastModel::linkByPath(const QDBusObjectPath &path)
{
    for (auto it(m_links.begin()); it != m_links.end(); ++it)
        if (it->m_dbusPath == path)
            return *it;

    Q_UNREACHABLE();
}
