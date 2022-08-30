// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "syncmodel.h"

using namespace dcc;
using namespace dcc::cloudsync;

SyncModel::SyncModel(QObject *parent)
    : QObject(parent)
    , m_syncIsValid(false)
    , m_enableSync(false)
    , m_activation(false)
    , m_lastSyncTime(0)
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

std::list<std::pair<SyncType, QStringList>> SyncModel::moduleMap()
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
    };
}

void SyncModel::setModuleSyncState(SyncType type, bool state)
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

QString SyncModel::userDisplayName() const
{
    QString username = m_userinfo["Nickname"].toString();
    if (username.isEmpty()) {
        username = m_userinfo["Username"].toString();
    }
    return username;
}

void SyncModel::setActivation(bool value)
{
    if (m_activation == value)
        return;
    m_activation = value;
    Q_EMIT licenseStateChanged(value);
}
