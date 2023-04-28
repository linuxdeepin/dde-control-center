//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MACHINEDBUSPROXY_H
#define MACHINEDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

#include <DDBusInterface>

class QDBusInterface;
class QDBusMessage;

using Dtk::Core::DDBusInterface;

class MachineDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MachineDBusProxy(QString cooperationMachinePath, QObject *parent = nullptr);

    Q_PROPERTY(QString IP READ ip NOTIFY IPChanged)
    QString ip();

    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

    Q_PROPERTY(bool Connected READ connected NOTIFY ConnectedChanged)
    bool connected();

    Q_PROPERTY(QString UUID READ uuid NOTIFY UUIDChanged)
    QString uuid();

    Q_PROPERTY(bool DeviceSharing READ deviceSharing NOTIFY DeviceSharingChanged)
    bool deviceSharing(); // 默认设备

    Q_PROPERTY(int Direction READ direction NOTIFY DirectionChanged)
    quint16 direction();

    void disconnect();
    void connect();
    void requestDeviceSharing();
    void setFlowDirection(quint16 direction);
    void stopDeviceSharing();

Q_SIGNALS:
    void IPChanged(const QString& ip);
    void NameChanged(const QString& name);
    void ConnectedChanged(bool connected);
    void UUIDChanged(const QString& uuid);
    void DeviceSharingChanged(bool cooperating);
    void DirectionChanged(int dir);

    void disconnectStatusChanged(bool);

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    DDBusInterface *m_dBusMachineInter;
    QString m_cooperationMachinePath;
};

#endif // MACHINEDBUSPROXY_H
