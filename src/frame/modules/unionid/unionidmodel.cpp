/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "unionidmodel.h"

using namespace dcc;
using namespace dcc::unionid;

UnionidModel::UnionidModel(QObject *parent)
    : QObject(parent)
    , m_syncIsValid(false)
    , m_enableSync(false)
{

}

void UnionidModel::setUserinfo(const QVariantMap &userinfo)
{
    if (m_userinfo == userinfo) return;

    m_userinfo = userinfo;

    Q_EMIT userInfoChanged(userinfo);
}

void UnionidModel::setSyncState(const std::pair<qint32, QString> &syncState)
{
    if (m_syncState == syncState) return;

    m_syncState = syncState;

    Q_EMIT syncStateChanged(syncState);
}

bool UnionidModel::isSyncStateValid(const std::pair<qint32, QString> &state)
{
    return state.first == 100 || state.first == 200 || state.first >= 1000;
}

bool UnionidModel::isSyncFailed(const std::pair<qint32, QString> &state)
{
    // NOTE(justforlxz): Daemon definition is greater than 1000 is wrong
    return state.first >= 1000;
}

bool UnionidModel::isSyncing(const std::pair<qint32, QString> &state)
{
    return state.first == 100;
}

bool UnionidModel::isSyncSucceed(const std::pair<qint32, QString> &state)
{
    return state.first == 200;
}


void UnionidModel::setEnableSync(bool enableSync)
{
    if (m_enableSync == enableSync) return;

    m_enableSync = enableSync;

    Q_EMIT enableSyncChanged(enableSync);
}


void UnionidModel::setModuleSyncState(dcc::cloudsync::SyncType type, bool state)
{
    m_moduleSyncState[type] = state;

    Q_EMIT moduleSyncStateChanged(std::pair<dcc::cloudsync::SyncType, bool>(type, state));
}

void UnionidModel::setSyncIsValid(bool syncIsValid)
{
    if (m_syncIsValid == syncIsValid) return;

    m_syncIsValid = syncIsValid;

    Q_EMIT syncIsValidChanged(syncIsValid);
}

QString UnionidModel::userDisplayName() const
{
    QString username = m_userinfo["Nickname"].toString();
    if (username.isEmpty()) {
        username = m_userinfo["Username"].toString();
    }
    return username;
}

QString UnionidModel::userEmail() const
{
    QString email = m_userinfo["Email"].toString();
    return email;
}
