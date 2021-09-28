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

#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H

#include "networkdevicebase.h"

namespace dde {

namespace network {

class AccessPoints;
class WirelessConnection;
/**
 * @brief 无线网络设备-无线网卡
 */
class WirelessDevice : public NetworkDeviceBase
{
    Q_OBJECT

    friend class NetworkController;

public:
    bool isConnected() const;                                       // 是否连接网络，重写基类的虚函数
    DeviceType deviceType() const;                                  // 返回设备类型，适应基类统一的接口
    QList<AccessPoints *> accessPointItems() const;                 // 当前网卡上所有的网络列表
    void scanNetwork();                                             // 重新加载所有的无线网络列表
    void connectNetwork(const AccessPoints *item);                  // 连接网络，连接成功抛出deviceStatusChanged信号
    void connectNetwork(const QString &ssid);                       // 连接网络，重载函数
    void disconnectNetwork();                                       // 断开连接
    QList<WirelessConnection *> items() const;                      // 无线网络连接列表
    AccessPoints *activeAccessPoints() const;                       // 当前活动的无线连接

Q_SIGNALS:
    void networkAdded(QList<AccessPoints *>);                       // wlan新增网络
    void networkRemoved(QList<AccessPoints *>);                     // wlan列表减少网络
    void connectionFailed(const AccessPoints *);                    // 连接无线wlan失败，第一个参数为失败的热点，第二个参数为对应的connection的Uuid
    void connectionSuccess(const AccessPoints *);                   // 连接无线网络wlan成功，参数为对应的wlan
    void hotspotEnableChanged(const bool &);                        // 热点是否可用发生变化
    void accessPointInfoChanged(const QList<AccessPoints *> &);     // wlan信号强度发生变化的网络

protected:
    WirelessDevice(NetworkInter *networkInter, QObject *parent);
    ~WirelessDevice();

private:
    AccessPoints *findAccessPoint(const QString &ssid);
    WirelessConnection *findConnectionByAccessPoint(const AccessPoints *accessPoint);
    void syncConnectionAccessPoints();
    void updateActiveInfo();

protected:
    void updateConnection(const QJsonArray &info);
    void createConnection(const QJsonArray &info);
    void updateActiveInfo(const QList<QJsonObject> &info);
    QString deviceKey();
    WirelessConnection *findConnectionByPath(const QString &path);
    void updateAccesspoint(const QJsonArray &json);
    void setDeviceEnabledStatus(const bool &enabled);
    void updateActiveConnectionInfo(const QList<QJsonObject> &infos);
    bool getHotspotEnabeld();

    template<class T>
    void clearListData(QList<T *> &dataList) {
        for (T *data : dataList)
            delete data;

        dataList.clear();
    }

private:
    QList<WirelessConnection *> m_connections;
    QList<AccessPoints *> m_accessPoints;
    QJsonObject m_activeHotspotInfo;
    QList<QJsonObject> m_activeAccessPoints;
    QJsonObject m_hotspotInfo;
    QJsonArray m_connectionJson;
};

/**
 * @brief 无线网络连接-wlan
 */

class AccessPoints : public QObject
{
    Q_OBJECT

    friend class WirelessDevice;

    Q_PROPERTY(QString ssid READ ssid)
    Q_PROPERTY(int strength READ strength)
    Q_PROPERTY(bool secured READ secured)
    Q_PROPERTY(bool securedInEap READ securedInEap)
    Q_PROPERTY(int frequency READ frequency)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QString devicePath READ devicePath)
    Q_PROPERTY(bool connected READ connected)

protected:
    AccessPoints(const QJsonObject &json, QObject *parent = Q_NULLPTR);
    ~AccessPoints();

public:
    QString ssid() const;                                           // 网络SSID，对应于返回接口中的Ssid
    int strength() const;                                           // 信号强度，对应于返回接口中的Strength
    bool secured() const;                                           // 是否加密，对应于返回接口中的Secured
    bool securedInEap() const;                                      // 对应于返回接口中的SecuredInEap
    int frequency() const;                                          // 频率，对应于返回接口中的Frequency
    QString path() const;                                           // 路径，对应于返回接口中的Path
    QString devicePath() const;                                     // 对应的设备的路径，为返回接口中的key值
    bool connected() const;                                         // 网络是否连接成功
    ConnectionStatus status() const;                                // 当前网络的连接状态

Q_SIGNALS:
    void strengthChanged(const int) const;                          // 当前信号强度变化

protected:
    void updateAccessPoints(const QJsonObject &json);

private:
    QJsonObject m_json;
    QString m_devicePath;
    ConnectionStatus m_status;
};

/**
 * @brief 无线连接信息
 */

class WirelessConnection: public ControllItems
{
    friend class WirelessDevice;

public:
    AccessPoints *accessPoints() const;                             // 返回当前对应的wlan的指针
    bool connected();                                               // 网络是否连接成功

protected:
    WirelessConnection();
    ~WirelessConnection();

    static WirelessConnection *createConnection(AccessPoints *ap);

private:
    Connection *m_connection;
    AccessPoints *m_accessPoints;
};

}

}

#endif // UWIRELESSDEVICE_H
