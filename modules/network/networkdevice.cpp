#include "networkdevice.h"

using namespace dcc::network;

NetworkDevice::NetworkDevice(const DeviceType type, const QJsonObject &data, QObject *parent)
    : QObject(parent),

      m_type(type),
      m_data(data)
{

}

NetworkDevice::NetworkDevice(const NetworkDevice &device)
    : NetworkDevice(device.type(), device.data(), device.parent())
{

}

void NetworkDevice::setEnabled(const bool enabled)
{
    if (m_enabled != enabled)
    {
        m_enabled = enabled;
        emit enableChanged(m_enabled);
    }
}

const QString NetworkDevice::path() const
{
    return m_data.value("Path").toString();
}
