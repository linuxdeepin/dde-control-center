//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "machinedbusproxy.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

MachineDBusProxy::MachineDBusProxy(QString cooperationMachinePath, QObject *parent)
    : QObject(parent)
    , m_cooperationMachinePath(cooperationMachinePath)
{
    const static QString CooperationService = "org.deepin.dde.Cooperation1";
    const static QString CooperationInterface = "org.deepin.dde.Cooperation1.Machine";

    m_dBusMachineInter = new DDBusInterface(CooperationService, m_cooperationMachinePath, CooperationInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection dbusConnection = m_dBusMachineInter->connection();
    dbusConnection.connect(CooperationService, m_cooperationMachinePath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

QString MachineDBusProxy::ip()
{
    return qvariant_cast<QString>(m_dBusMachineInter->property("IP"));
}

QString MachineDBusProxy::name()
{
    return qvariant_cast<QString>(m_dBusMachineInter->property("Name"));
}

bool MachineDBusProxy::connected()
{
    return qvariant_cast<bool>(m_dBusMachineInter->property("Connected"));
}

QString MachineDBusProxy::uuid()
{
    return qvariant_cast<QString>(m_dBusMachineInter->property("UUID"));
}

bool MachineDBusProxy::deviceSharing()
{
    return qvariant_cast<bool>(m_dBusMachineInter->property("DeviceSharing"));
}

quint16 MachineDBusProxy::direction()
{
    QDBusInterface in("org.deepin.dde.Cooperation1", m_cooperationMachinePath, "org.deepin.dde.Cooperation1.Machine", QDBusConnection::sessionBus());
    return in.property("Direction").toUInt();
}

void MachineDBusProxy::disconnect()
{
    qDebug() << " void MachineDBusProxy::Disconnect() " << qvariant_cast<QString>(m_dBusMachineInter->property("UUID"));
    QDBusReply<void> retPair = m_dBusMachineInter->call("Disconnect");
    QString errorDate = retPair.error().message();
    if (errorDate.isEmpty()) {
        Q_EMIT disconnectStatusChanged(true);
    } else {
        qWarning() << "Disconnect failed:" << errorDate;
    }
}

void MachineDBusProxy::connect()
{
    QDBusReply<void> retConnect = m_dBusMachineInter->call("Connect");
    QString errorDate = retConnect.error().message();
    if (!errorDate.isEmpty()) {
        qWarning() << "Connect failed:" << errorDate;
    }
}

void MachineDBusProxy::requestDeviceSharing()
{
    QDBusReply<void> retPair = m_dBusMachineInter->call("RequestDeviceSharing");
    QString errorDate = retPair.error().message();
    if (!errorDate.isEmpty()) {
        qWarning() << "RequestCooperate failed:" << errorDate;
    }
}

void MachineDBusProxy::setFlowDirection(quint16 direction)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(direction);
    m_dBusMachineInter->asyncCallWithArgumentList(QStringLiteral("SetFlowDirection"), argumentList);
}

void MachineDBusProxy::stopDeviceSharing()
{
    qDebug() << " StopDeviceSharing " << qvariant_cast<QString>(m_dBusMachineInter->property("UUID"));;
    QDBusReply<void> retPair = m_dBusMachineInter->call("StopDeviceSharing");
    QString errorDate = retPair.error().message();
    if (errorDate.isEmpty()) {
        Q_EMIT disconnectStatusChanged(true);
    } else {
        qWarning() << "Disconnect failed:" << errorDate;
    }
}

void MachineDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
