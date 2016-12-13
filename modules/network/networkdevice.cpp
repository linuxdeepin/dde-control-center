#include "networkdevice.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

using namespace dcc::network;

NetworkDevice::NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent)
    : QObject(parent),

      m_type(type),
      m_deviceInfo(info)
{

}

NetworkDevice::~NetworkDevice()
{
    emit removed();
}

void NetworkDevice::setEnabled(const bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enableChanged(m_enabled);
    }
}

const QString NetworkDevice::path() const
{
    return m_deviceInfo.value("Path").toString();
}

void NetworkDevice::updateDeviceInfo(const QJsonObject &devInfo)
{
    m_deviceInfo = devInfo;
}
