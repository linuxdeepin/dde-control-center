#ifndef WIREDDEVICE_H
#define WIREDDEVICE_H

#include "networkdevice.h"

#include <QJsonObject>
#include <QDBusObjectPath>

namespace dcc {

namespace network {

class WiredDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WiredDevice(const QJsonObject &info, QObject *parent = 0);

    void setConnections(const QList<QDBusObjectPath> &connections);
    const QList<QDBusObjectPath> connections() const { return m_connections; }
    const QJsonObject activeConnection() const { return m_activeConnection; }

signals:
    void connectionsChanged(const QList<QDBusObjectPath> &connPaths) const;
    void activeConnectionChanged(const QJsonObject &connInfo) const;

public slots:
    void onActiveConnectionChanged(const QJsonObject &connInfo);

private:
    QJsonObject m_activeConnection;
    QList<QDBusObjectPath> m_connections;
};

}

}

#endif // WIREDDEVICE_H
