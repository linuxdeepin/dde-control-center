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

const QString NetworkDevice::path() const
{
    return m_data.value("Path").toString();
}
