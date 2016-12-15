#include "networkworker.h"
#include "networkmodel.h"

using namespace dcc::network;

NetworkWorker::NetworkWorker(NetworkModel *model, QObject *parent)
    : QObject(parent),

      m_networkInter("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this),

      m_networkModel(model)
{
    connect(&m_networkInter, &NetworkInter::DevicesChanged, m_networkModel, &NetworkModel::onDeviceListChanged);
    connect(&m_networkInter, &NetworkInter::ConnectionsChanged, m_networkModel, &NetworkModel::onConnectionListChanged);
    connect(&m_networkInter, &NetworkInter::DeviceEnabled, m_networkModel, &NetworkModel::onDeviceEnableChaned);
    connect(&m_networkInter, &NetworkInter::AccessPointPropertiesChanged, m_networkModel, &NetworkModel::onDeviceAPInfoChanged);

    m_networkModel->onDeviceListChanged(m_networkInter.devices());
    m_networkModel->onConnectionListChanged(m_networkInter.connections());
}

void NetworkWorker::setDeviceEnable(const QString &devPath, const bool enable)
{
    m_networkInter.EnableDevice(QDBusObjectPath(devPath), enable);
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

    const QString uuid = m_networkModel->connectionUuid(connPath);
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
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter.IsDeviceEnabled(QDBusObjectPath(devPath)));

    w->setProperty("devPath", devPath);

    connect(w, &QDBusPendingCallWatcher::finished, this, &NetworkWorker::queryDeviceStatusCB);
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

    m_networkModel->onDeviceEnableChaned(w->property("devPath").toString(), reply.value());

    w->deleteLater();
}
