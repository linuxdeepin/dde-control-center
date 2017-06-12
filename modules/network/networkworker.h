#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QObject>

#include <com_deepin_daemon_network.h>

namespace dcc {

namespace network {

using NetworkInter = com::deepin::daemon::Network;

class NetworkModel;
class NetworkWorker : public QObject
{
    Q_OBJECT

public:
    explicit NetworkWorker(NetworkModel *model, QObject *parent = 0);

public slots:
    void setVpnEnable(const bool enable);
    void setDeviceEnable(const QString &devPath, const bool enable);
    void setProxyMethod(const QString &proxyMethod);
    void setProxyIgnoreHosts(const QString &hosts);
    void setAutoProxy(const QString &proxy);
    void setProxy(const QString &type, const QString &addr, const QString &port);
    void queryProxy(const QString &type);
    void queryAutoProxy();
    void queryProxyData();
    void queryProxyMethod();
    void queryProxyIgnoreHosts();
    void queryActiveConnInfo();
    void queryDevicesConnections();
    void queryAccessPoints(const QString &devPath);
    void queryConnectionSession(const QString &devPath, const QString &uuid);
    void queryDeviceStatus(const QString &devPath);
    void queryDeviceConnections(const QString &devPath);
    void deleteConnection(const QString &uuid);
    void deactiveConnection(const QString &uuid);
    void createApConfig(const QString &devPath, const QString &apPath);
    void createConnection(const QString &type, const QString &devPath);
    void activateConnection(const QString &devPath, const QString &uuid);
    void activateAccessPoint(const QString &devPath, const QString &apPath, const QString &uuid);

private slots:
    void queryAutoProxyCB(QDBusPendingCallWatcher *w);
    void queryProxyCB(QDBusPendingCallWatcher *w);
    void queryProxyMethodCB(QDBusPendingCallWatcher *w);
    void queryProxyIgnoreHostsCB(QDBusPendingCallWatcher *w);
    void queryAccessPointsCB(QDBusPendingCallWatcher *w);
    void queryConnectionSessionCB(QDBusPendingCallWatcher *w);
    void queryDeviceStatusCB(QDBusPendingCallWatcher *w);
    void queryDeviceConnectionsCB(QDBusPendingCallWatcher *w);
    void queryActiveConnInfoCB(QDBusPendingCallWatcher *w);

private:
    NetworkInter m_networkInter;

    NetworkModel *m_networkModel;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKWORKER_H
