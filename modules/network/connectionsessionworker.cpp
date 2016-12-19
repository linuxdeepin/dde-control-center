#include "connectionsessionworker.h"
#include "connectionsessionmodel.h"

using namespace dcc::network;

ConnectionSessionWorker::ConnectionSessionWorker(const QString &sessionPath, ConnectionSessionModel *model, QObject *parent)
    : QObject(parent),

      m_connModel(model),
      m_sessionInter("com.deepin.daemon.Network", sessionPath, QDBusConnection::sessionBus(), this)
{
    m_sessionInter.setSync(false);

//    connect(&m_sessionInter, &ConnectionSessionInter::AvailableSectionsChanged, m_connModel, &ConnectionSessionModel::setSections);
//    connect(&m_sessionInter, &ConnectionSessionInter::AvailableKeysChanged, m_connModel, &ConnectionSessionModel::setVisibleKeys);
    connect(&m_sessionInter, &ConnectionSessionInter::AvailableKeysChanged, this, &ConnectionSessionWorker::queryAvailableKeys);

//    m_connModel->setSections(m_sessionInter.availableSections());
//    m_connModel->setVisibleKeys(m_sessionInter.availableKeys());

    queryAllKeys();
    queryAvailableKeys();
}

void ConnectionSessionWorker::closeSession()
{
    if (m_sessionInter.isValid())
        m_sessionInter.Close();
}

void ConnectionSessionWorker::queryAllKeys()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_sessionInter.GetAllKeys(), this);

    connect(w, &QDBusPendingCallWatcher::finished, this, &ConnectionSessionWorker::queryAllKeysCB);
}

void ConnectionSessionWorker::queryAvailableKeys()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_sessionInter.ListAvailableKeyDetail(), this);

    connect(w, &QDBusPendingCallWatcher::finished, this, &ConnectionSessionWorker::queryAvailableKeysCB);
}

void ConnectionSessionWorker::queryAllKeysCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    m_connModel->setAllKeys(reply.value());

    w->deleteLater();
}

void ConnectionSessionWorker::queryAvailableKeysCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    m_connModel->setAvailableItems(reply.value());

    w->deleteLater();
}
