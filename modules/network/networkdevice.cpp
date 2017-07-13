#include "networkdevice.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

using namespace dcc::network;

NetworkDevice::NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent)
    : QObject(parent),

      m_type(type),
      m_status(Unknow),
      m_deviceInfo(info),
      m_enabled(true)
{
    updateDeviceInfo(info);
}

void NetworkDevice::setDeviceStatus(const int status)
{
    DeviceStatus stat = Unknow;

    switch (status)
    {
    case 10:    stat = Unmanaged;       break;
    case 20:    stat = Unavailable;     break;
    case 30:    stat = Disconnected;    break;
    case 40:    stat = Prepare;         break;
    case 50:    stat = Config;          break;
    case 60:    stat = NeedAuth;        break;
    case 70:    stat = IpConfig;        break;
    case 80:    stat = IpCheck;         break;
    case 90:    stat = Secondaries;     break;
    case 100:   stat = Activated;       break;
    case 110:   stat = Deactivation;    break;
    case 120:   stat = Failed;          break;
    }

    if (m_status != stat)
    {
        m_status = stat;

        emit statusChanged(m_status);
        emit statusChanged(statusString());
    }
}

const QString NetworkDevice::statusString() const
{
    switch (m_status)
    {
    case Unmanaged:
    case Unavailable:
    case Disconnected:  return tr("Disconnected");
    case Prepare:
    case Config:        return tr("Connecting");
    case NeedAuth:      return tr("Authenticating");
    case IpConfig:
    case IpCheck:       return tr("Obtaining Address");
    case Activated:     return tr("Connected");
    case Deactivation:
    case Failed:        return tr("Failed");
    default:;
    }

    return QString();
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

const QString NetworkDevice::hwAddr() const
{
    return m_deviceInfo.value("HwAddress").toString();
}

void NetworkDevice::updateDeviceInfo(const QJsonObject &devInfo)
{
    m_deviceInfo = devInfo;

    setDeviceStatus(m_deviceInfo.value("State").toInt());
}
