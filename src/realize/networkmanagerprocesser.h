#ifndef NETWORKMANAGERPROCESSER_H
#define NETWORKMANAGERPROCESSER_H

#include "netinterface.h"

namespace dde {
namespace network {

class NetworkDeviceBase;

class NetworkManagerProcesser : public NetworkProcesser
{
public:
    explicit NetworkManagerProcesser(QObject *parent = Q_NULLPTR);
    ~NetworkManagerProcesser();

protected:
    QList<NetworkDeviceBase *> devices();
    Connectivity connectivity();
    QList<NetworkDetails *> networkDetails();
    ProxyController *proxyController();                                          // 返回代理控制管理器
    VPNController *vpnController();                                              // 返回VPN控制器
    DSLController *dslController();                                              // DSL控制器
    HotspotController *hotspotController();                                      // 个人热点控制器

private:
    QList<NetworkDeviceBase *> m_devices;
    QList<NetworkDetails *> m_details;
    ProxyController *m_proxyController;
    VPNController *m_vpnController;
    DSLController *m_dslController;
    HotspotController *m_hotspotController;
};

}
}
#endif // NETWORKMANAGERPROCESSER_H
