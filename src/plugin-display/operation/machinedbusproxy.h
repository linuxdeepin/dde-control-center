//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MACHINEDBUSPROXY_H
#define MACHINEDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>
#include "widgets/dccdbusinterface.h"

class QDBusInterface;
class QDBusMessage;

namespace DCC_NAMESPACE {
class DCCDBusInterface;
}

class MachineDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MachineDBusProxy(QString cooperationMachinePath, QObject *parent = nullptr);

    Q_PROPERTY(QString IP READ IP NOTIFY IpChanged)
    QString IP();

    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

    Q_PROPERTY(bool Connected READ connected NOTIFY ConnectedChanged)
    bool connected();

    Q_PROPERTY(QString UUID READ UUID NOTIFY UuidChanged)
    QString UUID();

    Q_PROPERTY(bool DeviceSharing READ deviceSharing NOTIFY DeviceSharingChanged)
    bool deviceSharing(); // 默认设备

    Q_PROPERTY(int Direction READ direction NOTIFY directionChanged)
    quint16 direction();

    void disconnect();
    void connect();
    void requestDeviceSharing();
    void setFlowDirection(quint16 direction);
    void stopDeviceSharing();

Q_SIGNALS:
    void IpChanged(const QString& ip);
    void NameChanged(const QString& name);
    void ConnectedChanged(bool connecteded);
    void UuidChanged(const QString& uuid);
    void DeviceSharingChanged(bool cooperating);
    void disconnectStatusChanged(bool);
    void directionChanged(quint16 dir);

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    DCC_NAMESPACE::DCCDBusInterface *m_dBusMachineInter;
    QString m_cooperationMachinePath;
};

#endif // MACHINEDBUSPROXY_H
