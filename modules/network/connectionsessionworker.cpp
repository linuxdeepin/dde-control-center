#include "connectionsessionworker.h"

using namespace dcc::network;

ConnectionSessionWorker::ConnectionSessionWorker(const QString &sessionPath, ConnectionSessionModel *model, QObject *parent)
    : QObject(parent),

      m_connModel(model),
      m_sessionInter("com.deepin.daemon.Network", sessionPath, QDBusConnection::sessionBus(), this)
{
    qDebug() << m_sessionInter.GetAllKeys();
}
