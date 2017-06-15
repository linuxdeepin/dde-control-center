#include "wireddevice.h"

#include <QDebug>

using namespace dcc::network;

WiredDevice::WiredDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wired, info, parent)
{

}

void WiredDevice::setConnections(const QList<QDBusObjectPath> &connections)
{
    m_connections = connections;

    emit connectionsChanged(m_connections);
}

void WiredDevice::onActiveConnectionChanged(const QJsonObject &connInfo)
{
    m_activeConnection = connInfo;

    emit activeConnectionChanged(m_activeConnection);
}
