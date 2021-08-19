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

#ifndef NETWORKDEVICEBASE_H
#define NETWORKDEVICEBASE_H

#include "networkconst.h"

#include <QObject>

namespace dde {

namespace network {

#define MaxQueueSize 4

class NetworkDeviceBase : public QObject
{
    Q_OBJECT

    friend class NetworkController;

    Q_PROPERTY(QString statusName READ getStatusName)
    Q_PROPERTY(QString statusDetail READ statusStringDetail)                                    // 当前状态详细说明，取代statusStringDetail接口
    Q_PROPERTY(bool enabled READ isEnabled)                                                     // 当前网卡是否启用，对应于isEnabled() 接口，向外提供接口的同时，这里也给出属性
    Q_PROPERTY(bool IPValid READ IPValid)                                                       // IP是否合法，对应于isIPValid接口
    Q_PROPERTY(QString interface READ interface)                                                // 对应于interface() 接口
    Q_PROPERTY(QString driver READ driver)                                                      // 对应于driver()接口
    Q_PROPERTY(bool managed READ managed)                                                       // 对应于managed接口
    Q_PROPERTY(QString vendor READ vendor)                                                      // 对应于vendor接口
    Q_PROPERTY(QString uniqueUuid READ uniqueUuid)                                              // 对应于uniqueUuid接口
    Q_PROPERTY(bool usbDevice READ usbDevice)                                                   // 对应于usbDevice接口
    Q_PROPERTY(QString path READ path)                                                          // 对应于path接口
    Q_PROPERTY(QString activeAp READ activeAp)                                                  // 对应于ActiveAp接口
    Q_PROPERTY(bool supportHotspot READ supportHotspot)                                         // 对应于supportHotspot接口
    Q_PROPERTY(QString realHwAdr READ realHwAdr)                                                // 对应于realHwAdr接口
    Q_PROPERTY(QString usingHwAdr READ usingHwAdr)                                              // 对应于usingHwAdr接口
    Q_PROPERTY(QString ipv4 READ ipv4)                                                          // 对应于ipv4接口
    Q_PROPERTY(QString ipv6 READ ipv6)                                                          // 对应于ipv6接口
    Q_PROPERTY(QString deviceName READ deviceName WRITE setName NOTIFY nameChanged)             // 对应于设备名称

Q_SIGNALS:
    void deviceStatusChanged(const DeviceStatus &) const;                                        // 状态发生变化的时候触发的信号
    void enableChanged(const bool) const;                                                        // 开启设备或禁用设备的时候发出的信号(参数值True表示开启设备，False表示禁用设备)
    void connectionChanged();                                                                    // 连接发生变化的时候触发的信号
    void nameChanged(const QString &);                                                           // 网卡名称发生变化的时候触发的信号

public:
    inline bool isEnabled() const { return m_enabled; }                                          // 当前的网卡是否启用
    virtual bool isConnected() const = 0;                                                        // 当前网络的网络是否处于连接状态
    bool IPValid();                                                                              // IP是否合法
    virtual DeviceType deviceType() const = 0;                                                   // 设备类型 未识别 无线网卡 有线网卡，直接在子类中返回当前的类型即可
    inline DeviceStatus deviceStatus() const { return m_deviceStatus; }                          // 设备状态
    inline QString interface() const { return m_data.value("Interface").toString(); }            // 返回设备上的Interface
    inline QString driver() const { return m_data.value("Driver").toString(); }                  // 驱动，对应于备上返回值的Driver
    inline bool managed() const { return m_data.value("Managed").toBool(); }                     // 对应于设备上返回值的Managed
    inline QString vendor() const { return m_data.value("Vendor").toString(); }                  // 对应于设备上返回值的Vendor
    inline QString uniqueUuid() const { return m_data.value("UniqueUuid").toString(); }          // 网络设备的唯一的UUID，对应于设备上返回值的UniqueUuid
    inline bool usbDevice() const { return m_data.value("UsbDevice").toBool(); }                 // 是否是USB设备，对应于设备上返回值的UsbDevice
    inline QString path() const { return m_data.value("Path").toString(); }                      // 设备路径，对应于设备上返回值的Path
    inline QString activeAp() const { return m_data.value("ActiveAp").toString(); }              // 对应于设备上返回值的ActiveAp
    inline bool supportHotspot() const { return m_data.value("SupportHotspot").toBool(); }       // 是否支持热点,对应于设备上返回值的SupportHotspot
    inline QString realHwAdr() const { return m_data.value("HwAddress").toString(); }            // mac地址
    inline QString usingHwAdr() const { return m_data.value("ClonedAddress").toString(); }       // 正在使用的mac地址
    QString ipv4() const;                                                                        // IPV4地址
    QString ipv6() const;                                                                        // IPV6地址
    QJsonObject activeConnectionInfo() const;                                                    // 获取当前活动连接的信息
    void setEnabled(bool enabled);                                                               // 开启或禁用网卡
    virtual void disconnectNetwork() = 0;                                                        // 断开网络连接，该方法是一个虚方法，具体在子类
    Connectivity connectivity();
    virtual void setName(const QString &name);                                                   // 设置设备的名称
    virtual QString deviceName();                                                                // 设备名称

protected:
    explicit NetworkDeviceBase(NetworkInter *networkInter, QObject *parent = Q_NULLPTR);
    virtual ~NetworkDeviceBase();
    NetworkInter *networkInter();
    void updateDeviceInfo(const QJsonObject &info);
    void initDeviceInfo();
    virtual void updateConnection(const QJsonArray &info) = 0;
    virtual QString deviceKey() = 0;                                                             // 返回设备对应的key值
    virtual void setDeviceEnabledStatus(const bool &enabled);
    virtual void updateActiveInfo(const QList<QJsonObject> &info);                               // 当前连接发生变化，例如从一个连接切换到另外一个连接
    void updateActiveConnectionInfo(const QList<QJsonObject> &infos);                            // 当前连接发生变化后，获取设备的活动信息，例如IP等
    void enqueueStatus(const DeviceStatus &status);

protected:
    QString getStatusName();
    QString statusStringDetail();

private:
    NetworkInter *m_networkInter;
    QJsonObject m_data;
    QJsonObject m_activeInfoData;
    DeviceStatus m_deviceStatus;
    bool m_enabled;
    Connectivity m_connectivity;
    QQueue<DeviceStatus> m_statusQueue;
    QString m_name;
};

}

}

#endif // UNETWORKDEVICEBASE_H
