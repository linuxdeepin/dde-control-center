#ifndef IPCONFILCTCHECKER_H
#define IPCONFILCTCHECKER_H

#include <QObject>
#include "netutils.h"

#include "networkconst.h"

namespace dde {
namespace network {

class NetworkDeviceBase;
class NetworkProcesser;
class DeviceIPChecker;

class IPConfilctChecker : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void conflictStatusChanged(NetworkDeviceBase *, const bool &);

public:
    explicit IPConfilctChecker(NetworkProcesser *networkProcesser, const bool ipChecked, QObject *parent = nullptr);
    ~IPConfilctChecker();

public Q_SLOT:
    bool ipConfilct(NetworkDeviceBase *device) const;

private Q_SLOT:
    void onDeviceAdded(QList<NetworkDeviceBase *> devices);
    void onIPConfilct(const QString &ip, const QString &macAddress);
    void onSenderIPInfo(const QStringList &ips);

private:
    void handlerIpConflict(const QString &ip, const QString &macAddress, const QString &activeConnectionInfo);
    QMap<QString, NetworkDeviceBase *> parseDeviceIp(const QString &activeConnectionInfo);
    void clearUnExistDevice();

private:
    NetworkInter *m_networkInter;
    NetworkProcesser *m_networkProcesser;
    QList<DeviceIPChecker *> m_deviceCheckers;
    bool m_ipNeedCheck;
    QThread *m_thread;
};

class DeviceIPChecker : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void conflictStatusChanged(NetworkDeviceBase *, const bool &);
    void ipConflictCheck(const QStringList &);

public:
    explicit DeviceIPChecker(NetworkDeviceBase *device, NetworkInter *netInter, QObject *parent);
    ~DeviceIPChecker();
    NetworkDeviceBase *device();
    void setDeviceInfo(const QStringList &ipv4, const QString &macAddress);
    bool ipConfilct();
    void handlerIpConflict();
    QStringList ipV4();

private:
    NetworkDeviceBase *m_device;
    NetworkInter *m_networkInter;
    QStringList m_ipV4;
    QString m_macAddress;
    int m_conflictCount;
    int m_clearCount;
    bool m_ipConflicted;
    QList<QStringList> m_changeIpv4s;
};

}
}

#endif // IPCONFILCTCHECKER_H
