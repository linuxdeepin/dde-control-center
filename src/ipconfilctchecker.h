#ifndef IPCONFILCTCHECKER_H
#define IPCONFILCTCHECKER_H

#include <QObject>

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
    explicit IPConfilctChecker(NetworkProcesser *networkProcesser, const bool ipChecked, NetworkInter *netInter = nullptr, QObject *parent = nullptr);
    ~IPConfilctChecker();

    bool ipConfilct(NetworkDeviceBase *device) const;

private Q_SLOT:
    void onIPConfilct(const QString &ip, const QString &macAddress);

private:
    void handlerIpConflict(const QString &ip, const QString &macAddress, const QString &activeConnectionInfo);
    QMap<QString, NetworkDeviceBase *> parseDeviceIp(const QString &activeConnectionInfo);
    void clearUnExistDevice();

private:
    NetworkInter *m_networkInter;
    NetworkProcesser *m_networkProcesser;
    QList<DeviceIPChecker *> m_deviceCheckers;
    bool m_ipChecked;
};

class DeviceIPChecker : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void conflictStatusChanged(NetworkDeviceBase *, const bool &);

public:
    explicit DeviceIPChecker(NetworkDeviceBase *device, NetworkInter *netInter, const bool &ipCheck, QObject *parent);
    ~DeviceIPChecker();
    NetworkDeviceBase *device();
    void setDeviceInfo(const QString &ipv4, const QString &macAddress);
    bool ipConfilct();
    void handlerIpConflict();
    QString ipV4();

private:
    void requestIPConflictCheck(const QString &ipv4, const int interval = -1);

private:
    NetworkDeviceBase *m_device;
    NetworkInter *m_networkInter;
    QString m_ipV4;
    QString m_macAddress;
    int m_conflictCount;
    int m_clearCount;
    bool m_ipConflicted;
    bool m_ipCheck;
    QStringList m_changeIpv4s;
};

}
}

#endif // IPCONFILCTCHECKER_H
