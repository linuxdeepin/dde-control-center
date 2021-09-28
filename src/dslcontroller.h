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

#ifndef DSLCONTROLLER_H
#define DSLCONTROLLER_H

#include "networkconst.h"

#include <QObject>

namespace dde {

namespace network {

class DSLItem;
class NetworkDeviceBase;

class DSLController : public QObject
{
    Q_OBJECT

    friend class NetworkController;

public:
    void connectItem(DSLItem *item);                                      // 连接到当前的DSL
    void connectItem(const QString &uuid);                                // 根据UUID连接当前DSL
    void disconnectItem();                                                // 断开连接
    inline QList<DSLItem *> items() const { return m_items; }             // 返回所有的DSL列表

Q_SIGNALS:
    void itemAdded(const QList<DSLItem *> &);                             // 新增DSL项目
    void itemRemoved(const QList<DSLItem *> &);                           // 移除DSL项目
    void itemChanged(const QList<DSLItem *> &);                           // 项目发生变化（一般是ID发生了变化）
    void activeConnectionChanged();                                       // 连接状态发生变化

protected:
    explicit DSLController(NetworkInter *networkInter, QObject *parent = Q_NULLPTR);
    ~DSLController();

    void updateDevice(const QList<NetworkDeviceBase *> &devices);
    void updateDSLItems(const QJsonArray &dsljson);
    void updateActiveConnections(const QJsonObject &connectionJson);       // 更新DSL的活动连接信息

private:
    DSLItem *findItem(const QString &path);
    DSLItem *findDSLItemByUuid(const QString &uuid);

private:
    QList<DSLItem *> m_items;
    NetworkInter *m_networkInter;
    QMap<QString, QString> m_deviceInfo;
    QString m_activePath;
};

class DSLItem : public ControllItems
{
    friend class DSLController;

protected:
    DSLItem();
    ~DSLItem();

    void setConnectionStatus(ConnectionStatus connectStatus);

public:
    ConnectionStatus status() const;                             // 当前连接的连接状态

private:
    ConnectionStatus m_connectStatus;
};

}

}

#endif // UDSLCONTROLLER_H
