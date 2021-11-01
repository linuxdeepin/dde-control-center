#include "dslcontroller.h"
#include "hotspotcontroller.h"
#include "networkdevicebase.h"
#include "networkmanagerprocesser.h"
#include "proxycontroller.h"
#include "vpncontroller.h"

#include <networkmanagerqt/wireddevice.h>
#include <networkmanagerqt/manager.h>

namespace dde {
namespace network {

using namespace NetworkManager;

NetworkManagerProcesser::NetworkManagerProcesser(QObject *parent)
    : NetworkProcesser(parent)
    , m_proxyController(nullptr)
    , m_vpnController(nullptr)
    , m_dslController(nullptr)
    , m_hotspotController(nullptr)
{
}

NetworkManagerProcesser::~NetworkManagerProcesser()
{
}

QList<NetworkDeviceBase *> NetworkManagerProcesser::devices()
{
    return m_devices;
}

Connectivity NetworkManagerProcesser::connectivity()
{
    return Connectivity::Full;
}

QList<NetworkDetails *> NetworkManagerProcesser::networkDetails()
{
    return m_details;
}

ProxyController *NetworkManagerProcesser::proxyController()
{
    if (!m_proxyController)
        m_proxyController = new ProxyController(nullptr, this);

    return m_proxyController;
}

VPNController *NetworkManagerProcesser::vpnController()
{
    if (!m_vpnController)
        m_vpnController = new VPNController(nullptr, this);

    return m_vpnController;
}

DSLController *NetworkManagerProcesser::dslController()
{
    if (!m_dslController)
        m_dslController = new DSLController(nullptr, this);

    return m_dslController;
}

HotspotController *NetworkManagerProcesser::hotspotController()
{
    if (!m_hotspotController)
        m_hotspotController = new HotspotController(nullptr, this);

    return m_hotspotController;
}

}
}
