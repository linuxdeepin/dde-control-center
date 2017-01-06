#include "networkworker.h"
#include "networkmodel.h"

using namespace dcc::network;

NetworkWorker::NetworkWorker(NetworkModel *model, QObject *parent)
    : QObject(parent),

      m_networkInter("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this),

      m_networkModel(model)
{
    connect(&m_networkInter, &NetworkInter::ActiveConnectionsChanged, this, &NetworkWorker::queryActiveConnInfo);
    connect(&m_networkInter, &NetworkInter::DevicesChanged, m_networkModel, &NetworkModel::onDeviceListChanged);
    connect(&m_networkInter, &NetworkInter::ConnectionsChanged, m_networkModel, &NetworkModel::onConnectionListChanged);
    connect(&m_networkInter, &NetworkInter::DeviceEnabled, m_networkModel, &NetworkModel::onDeviceEnableChanged);
    connect(&m_networkInter, &NetworkInter::AccessPointAdded, m_networkModel, &NetworkModel::onDeviceAPInfoChanged);
    connect(&m_networkInter, &NetworkInter::AccessPointPropertiesChanged, m_networkModel, &NetworkModel::onDeviceAPInfoChanged);
    connect(&m_networkInter, &NetworkInter::AccessPointRemoved, m_networkModel, &NetworkModel::onDeviceAPRemoved);
    connect(&m_networkInter, &NetworkInter::VpnEnabledChanged, m_networkModel, &NetworkModel::onVPNEnabledChanged);
    connect(m_networkModel, &NetworkModel::requestDeviceStatus, this, &NetworkWorker::queryDeviceStatus, Qt::QueuedConnection);

    m_networkModel->onDeviceListChanged(m_networkInter.devices());
    m_networkModel->onConnectionListChanged(m_networkInter.connections());
    m_networkModel->onVPNEnabledChanged(m_networkInter.vpnEnabled());

    m_networkInter.setSync(false);

    queryActiveConnInfo();
}

void NetworkWorker::setVpnEnable(const bool enable)
{
    m_networkInter.setVpnEnabled(enable);
}

void NetworkWorker::setDeviceEnable(const QString &devPath, const bool enable)
{
    m_networkInter.EnableDevice(QDBusObjectPath(devPath), enable);
}

void NetworkWorker::queryActiveConnInfo()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter.GetActiveConnectionInfo(), this);

    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryActiveConnInfoCB);
}

void NetworkWorker::queryAccessPoints(const QString &devPath)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter.GetAccessPoints(QDBusObjectPath(devPath)));

    w->setProperty("devPath", devPath);

    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryAccessPointsCB);
}

void NetworkWorker::queryConnectionSession(const QString &devPath, const QString &connPath)
{
    QDBusPendingReply<QDBusObjectPath> reply;

    const QString uuid = m_networkModel->connectionUuidByPath(connPath);
    if (!uuid.isEmpty())
    {
        reply = m_networkInter.EditConnection(uuid, QDBusObjectPath(devPath));
    }
    else
    {
        // only support ap connection
        Q_ASSERT(connPath.contains("AccessPoint"));

        reply = m_networkInter.CreateConnectionForAccessPoint(QDBusObjectPath(connPath), QDBusObjectPath(devPath));
    }

    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(reply);

    w->setProperty("devPath", devPath);

    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryConnectionSessionCB);
}

void NetworkWorker::queryDeviceStatus(const QString &devPath)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter.IsDeviceEnabled(QDBusObjectPath(devPath)), this);

    w->setProperty("devPath", devPath);

    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryDeviceStatusCB);
}

void NetworkWorker::createConnection(const QString &type, const QString &devPath)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter.CreateConnection(type, QDBusObjectPath(devPath)));

    w->setProperty("devPath", devPath);

    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryConnectionSessionCB);
}

void NetworkWorker::activateConnection(const QString &devPath, const QString &uuid)
{
    m_networkInter.ActivateConnection(uuid, QDBusObjectPath(devPath));
}

void NetworkWorker::activateAccessPoint(const QString &devPath, const QString &apPath, const QString &ssid)
{
    const QString uuid = m_networkModel->connectionUuidBySsid(ssid);

    qDebug() << uuid;

    m_networkInter.ActivateAccessPoint(uuid, QDBusObjectPath(apPath), QDBusObjectPath(devPath));
}

void NetworkWorker::queryAccessPointsCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    m_networkModel->onDeviceAPListChanged(w->property("devPath").toString(), reply.value());

    w->deleteLater();
}

void NetworkWorker::queryConnectionSessionCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QDBusObjectPath> reply = *w;

    m_networkModel->onConnectionSessionCreated(w->property("devPath").toString(), reply.value().path());

    w->deleteLater();
}

void NetworkWorker::queryDeviceStatusCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<bool> reply = *w;

    m_networkModel->onDeviceEnableChanged(w->property("devPath").toString(), reply.value());

    w->deleteLater();
}

void NetworkWorker::queryActiveConnInfoCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    m_networkModel->onActiveConnInfoChanged(reply.value());

    w->deleteLater();
}
