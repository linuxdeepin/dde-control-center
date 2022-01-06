#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include "networkconst.h"

namespace dde {
  namespace network {
    class NetworkDeviceBase;
    class VPNItem;
    class AccessPoints;
  }
}

using namespace dde::network;

class NetworkHandler : public QObject
{
    Q_OBJECT

public:
    explicit NetworkHandler(QObject *parent = Q_NULLPTR);
    ~NetworkHandler();

private:
    void initConnection();
    void initProxy();
    void initHotsPot();
    void initDSL();

protected:
    void onDeviceInput() const;

private Q_SLOTS:
    void onDeviceAdded(QList<NetworkDeviceBase *> devices);
    void onDeviceRemoved(QList<NetworkDeviceBase *> devices);

    void onEnableChanged(const bool enabled) const;
    void onConnectionChanged();
    void onConnectivityChanged();

    void onVpnEnabled(const bool enabled);
    void onItemAdded(const QList<VPNItem *> vpns);
    void onItemRemoved(const QList<VPNItem *> vpns);

    void onAutoProxyChanged(const QString &proxy);
    void onProxyChanged(const SysProxyConfig &proxy);
    void onProxyMethodChange(const ProxyMethod &method);
    void onProxyIgnoreHostsChanged(const QString &host);
    void onAppProxyChanged();

    void onHotspotChanged();
    void onStatusChanged();
    void onHotspotEnabledChanged(const bool &enabled);

    void onDSLActiveChanged();
    void onVPNActiveChanged();

    void onNetworkAdded(QList<AccessPoints*> newAps);
    void onNetworkRemoved(QList<AccessPoints*> rmAps);
};

#endif // NETWORKHANDLER_H
