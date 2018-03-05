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

#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include "networkmodel.h"

#include <QObject>

#include <com_deepin_daemon_network.h>
#include <com_deepin_daemon_network_proxychains.h>

namespace dcc {

namespace network {

using NetworkInter = com::deepin::daemon::Network;
using ProxyChains = com::deepin::daemon::network::ProxyChains;

class NetworkWorker : public QObject
{
    Q_OBJECT

public:
    explicit NetworkWorker(NetworkModel *model, QObject *parent = 0);

    void active();

public slots:
    void activateConnection(const QString &devPath, const QString &uuid);
    void activateAccessPoint(const QString &devPath, const QString &apPath, const QString &uuid);
    void createApConfig(const QString &devPath, const QString &apPath);
    void createConnection(const QString &type, const QString &devPath);
    void deleteConnection(const QString &uuid);
    void deactiveConnection(const QString &uuid);
    void initWirelessHotspot(const QString &devPath);
    void queryChains();
    void queryAutoProxy();
    void queryProxyData();
    void queryProxyMethod();
    void queryProxyIgnoreHosts();
    void queryActiveConnInfo();
//    void queryDevicesConnections();
    void queryProxy(const QString &type);
    void queryAccessPoints(const QString &devPath);
    void queryConnectionSession(const QString &devPath, const QString &uuid);
    void queryDeviceStatus(const QString &devPath);
//    void queryDeviceConnections(const QString &devPath);
    void remanageDevice(const QString &devPath);
    void setVpnEnable(const bool enable);
    void setDeviceEnable(const QString &devPath, const bool enable);
    void setProxyMethod(const QString &proxyMethod);
    void setProxyIgnoreHosts(const QString &hosts);
    void setAutoProxy(const QString &proxy);
    void setProxy(const QString &type, const QString &addr, const QString &port);
    void setChainsProxy(const ProxyConfig &config);

private slots:
    void queryAutoProxyCB(QDBusPendingCallWatcher *w);
    void queryProxyCB(QDBusPendingCallWatcher *w);
    void queryProxyMethodCB(QDBusPendingCallWatcher *w);
    void queryProxyIgnoreHostsCB(QDBusPendingCallWatcher *w);
    void queryAccessPointsCB(QDBusPendingCallWatcher *w);
    void queryConnectionSessionCB(QDBusPendingCallWatcher *w);
    void queryDeviceStatusCB(QDBusPendingCallWatcher *w);
//    void queryDeviceConnectionsCB(QDBusPendingCallWatcher *w);
    void queryActiveConnInfoCB(QDBusPendingCallWatcher *w);

private:
    NetworkInter m_networkInter;
    ProxyChains *m_chainsInter;
    NetworkModel *m_networkModel;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKWORKER_H
