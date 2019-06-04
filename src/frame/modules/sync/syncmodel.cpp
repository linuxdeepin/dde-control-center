#include "syncmodel.h"

using namespace dcc;
using namespace dcc::cloudsync;

SyncModel::SyncModel(QObject *parent)
    : QObject(parent)
    , m_syncIsValid(false)
    , m_enableSync(false)
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

bool SyncModel::isSyncStateValid(const std::pair<qint32, QString> &state)
{
    return state.first == 100 || state.first == 200 || state.first >= 1000;
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

void SyncModel::setEnableSync(bool enableSync)
{
    if (m_enableSync == enableSync) return;

    m_enableSync = enableSync;

    Q_EMIT enableSyncChanged(enableSync);
}

std::list<std::pair<SyncModel::SyncType, QStringList>> SyncModel::moduleMap()
{
    return std::list<std::pair<SyncType, QStringList>>{
        { Network, { "network" } },
        { Sound, { "audio" } },
        { Mouse, { "peripherals" } },
        { Update, { "updater" } },
        { Dock, { "dock" } },
        { Launcher, { "launcher" } },
        { Wallpaper, { "background", "screensaver" } },
        { Theme, { "appearance" } },
        { Power, { "power" } },
        { Corner, { "screen_edge" } }
    };
}

void SyncModel::setModuleSyncState(SyncModel::SyncType type, bool state)
{
    m_moduleSyncState[type] = state;

    Q_EMIT moduleSyncStateChanged(std::pair<SyncType, bool>(type, state));
}

void SyncModel::setSyncIsValid(bool syncIsValid)
{
    if (m_syncIsValid == syncIsValid) return;

    m_syncIsValid = syncIsValid;

    Q_EMIT syncIsValidChanged(syncIsValid);
}
