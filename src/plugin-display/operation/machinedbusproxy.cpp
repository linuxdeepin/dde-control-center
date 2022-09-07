#include "machinedbusproxy.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

MachineDBusProxy::MachineDBusProxy(QString cooperationMachinePath, QObject *parent)
    : QObject(parent)
    , m_cooperationMachinePath(cooperationMachinePath)
{
    init();
}

QString MachineDBusProxy::IP()
{
    return qvariant_cast<QString>(m_dBusMachineInter->property("IP"));
}

QString MachineDBusProxy::name()
{
    return qvariant_cast<QString>(m_dBusMachineInter->property("Name"));
}

bool MachineDBusProxy::paired()
{
    return qvariant_cast<bool>(m_dBusMachineInter->property("Paired"));
}

QString MachineDBusProxy::UUID()
{
    return qvariant_cast<QString>(m_dBusMachineInter->property("UUID"));
}

bool MachineDBusProxy::cooperating()
{
    return qvariant_cast<bool>(m_dBusMachineInter->property("Cooperating"));
}

void MachineDBusProxy::Disconnect()
{
    m_dBusMachineInter->asyncCall(QStringLiteral("Disconnect"));
}

void MachineDBusProxy::Pair()
{
    QDBusReply<void> retPair = m_dBusMachineInter->call("Pair");
    QString errorDate = retPair.error().message();
    if (!errorDate.isEmpty()) {
        qWarning() << "Pair failed:" << errorDate;
    }
}

void MachineDBusProxy::RequestCooperate()
{
    QDBusReply<void> retPair = m_dBusMachineInter->call("RequestCooperate");
    QString errorDate = retPair.error().message();
    if (!errorDate.isEmpty()) {
        qWarning() << "RequestCooperate failed:" << errorDate;
    }
}

void MachineDBusProxy::init()
{
    const static QString CooperationService = "com.deepin.Cooperation";
    const static QString CooperationInterface = "com.deepin.Cooperation.Machine";
    const QString PropertiesInterface = "org.freedesktop.DBus.Properties";
    const QString PropertiesChanged = "PropertiesChanged";

    m_dBusMachineInter = new QDBusInterface(CooperationService, m_cooperationMachinePath, CooperationInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection dbusConnection = m_dBusMachineInter->connection();
    dbusConnection.connect(CooperationService, m_cooperationMachinePath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

void MachineDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
