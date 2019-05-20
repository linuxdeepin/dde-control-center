#include "syncmodel.h"

using namespace dcc;
using namespace dcc::sync;

SyncModel::SyncModel(QObject *parent) : QObject(parent)
{

}

void SyncModel::setUserinfo(const QVariantMap &userinfo)
{
    if (m_userinfo == userinfo) return;

    m_userinfo = userinfo;

    Q_EMIT userInfoChanged(userinfo);
}

void SyncModel::setSyncState(const std::pair<qint32, QString> &syncState)
{
    if (m_syncState == syncState) return;

    m_syncState = syncState;

    Q_EMIT syncStateChanged(syncState);
}

bool SyncModel::isSyncFailed(const std::pair<qint32, QString> &state)
{
    // NOTE(justforlxz): Daemon definition is greater than 1000 is wrong
    return state.first >= 1000;
}

bool SyncModel::isSyncing(const std::pair<qint32, QString> &state)
{
    return state.first == 100;
}

bool SyncModel::isSyncSucceed(const std::pair<qint32, QString> &state)
{
    return state.first == 200;
}

void SyncModel::setLastSyncTime(const qlonglong &lastSyncTime)
{
    if (m_lastSyncTime == lastSyncTime) return;

    m_lastSyncTime = lastSyncTime;

    Q_EMIT lastSyncTimeChanged(lastSyncTime);
}
