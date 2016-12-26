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
    void queryAccessPoints(const QString &devPath);
    void queryConnectionSession(const QString &devPath, const QString &connPath);
    void queryDeviceStatus(const QString &devPath);
    void createConnection(const QString &type, const QString &devPath);

private:
    void queryAccessPointsCB(QDBusPendingCallWatcher *w);
    void queryConnectionSessionCB(QDBusPendingCallWatcher *w);
    void queryDeviceStatusCB(QDBusPendingCallWatcher *w);

private:
    NetworkInter m_networkInter;

    NetworkModel *m_networkModel;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKWORKER_H
