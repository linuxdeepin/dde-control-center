/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

const QString NetworkDevice::realHwAdr() const
{
    return m_deviceInfo.value("HwAddress").toString();
}

const QString NetworkDevice::usingHwAdr() const
{
    const auto &hwAdr = m_deviceInfo.value("HwAddress").toString();
    const auto &clonedAdr = m_deviceInfo.value("ClonedAddress").toString();

    return clonedAdr.isEmpty() ? hwAdr : clonedAdr;
}

void NetworkDevice::updateDeviceInfo(const QJsonObject &devInfo)
{
    m_deviceInfo = devInfo;

    setDeviceStatus(m_deviceInfo.value("State").toInt());
}
