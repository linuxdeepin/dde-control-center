/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#ifndef VPNCONTROLLER_H
#define VPNCONTROLLER_H

#include "networkconst.h"

#include <QObject>

namespace dde {

namespace network {

class VPNItem;

class VPNController : public QObject
{
    Q_OBJECT

    friend class NetworkController;

public:
    inline void setEnabled(const bool enabled) { m_networkInter->setVpnEnabled(enabled); }       // 开启或者关闭VPN
    inline bool enabled() const { return m_enabled; }                                            // VPN开启还是关闭
    void connectItem(VPNItem *item);                                                             // 连接VPN
    void connectItem(const QString &uuid);                                                       // 连接VPN(重载函数)
    void disconnectItem();                                                                       // 断开当前活动VPN连接
    inline QList<VPNItem *> items() { return m_vpnItems; }                                       // 获取所有的VPN列表

Q_SIGNALS:
    void enableChanged(const bool);                                                              // 开启关闭VPN发出的信号
    void itemAdded(const QList<VPNItem *> &);                                                    // 新增VPN发出的信号
    void itemRemoved(const QList<VPNItem *> &);                                                  // 移除VPN发出的信号
    void itemChanged(const QList<VPNItem *> &);                                                  // VPN项发生变化（ID）
    void activeConnectionChanged();                                                              // 活动连接发生变化的时候发出的信号

protected:
    explicit VPNController(NetworkInter *networkInter, QObject *parent = Q_NULLPTR);
    ~VPNController();

    void updateVPNItems(const QJsonArray &vpnArrays);
    void updateActiveConnection(const QJsonObject &activeConection);

private:
    VPNItem *findItem(const QString &path);
    VPNItem *findItemByUuid(const QString &uuid);

private Q_SLOTS:
    void onEnableChanged(const bool enabled);

private:
    NetworkInter *m_networkInter;
    bool m_enabled;
    QList<VPNItem *> m_vpnItems;
    QString m_activePath;
};

class VPNItem : public ControllItems
{
    friend class VPNController;

public:
    ConnectionStatus connectionStatus() const;                                              // 当前连接的连接状态

protected:
    VPNItem();
    ~VPNItem();

    void setConnectionStatus(ConnectionStatus connectionStatus);

private:
    ConnectionStatus m_connectionStatus;
};

}

}

#endif // UVPNCONTROLLER_H
