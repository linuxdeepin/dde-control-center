/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef SYNCMODEL_H
#define SYNCMODEL_H

#include "utils.h"

#include <QObject>
#include <QVariantMap>
#include <utility>

class SyncModel : public QObject {
    Q_OBJECT
public:
    explicit SyncModel(QObject *parent = nullptr);

    inline const QVariantMap userinfo() const { return m_userinfo; }
    void setUserinfo(const QVariantMap &userinfo);

    inline std::pair<qint32, QString> syncState() const { return m_syncState; }
    void setSyncState(const std::pair<qint32, QString> &syncState);

    static bool isSyncStateValid(const std::pair<qint32, QString> &state);
    static bool isSyncSucceed(const std::pair<qint32, QString> &state);
    static bool isSyncFailed(const std::pair<qint32, QString> &state);
    static bool isSyncing(const std::pair<qint32, QString> &state);

    inline qlonglong lastSyncTime() const { return m_lastSyncTime; }
    void setLastSyncTime(const qlonglong &lastSyncTime);

    inline bool enableSync() const { return m_enableSync; }
    void setEnableSync(bool enableSync);

    static std::list<std::pair<SyncType, QStringList>> moduleMap();

    inline QMap<SyncType, bool> moduleSyncState() const { return m_moduleSyncState; }
    inline bool getModuleStateByType(SyncType type) {
        return m_moduleSyncState[type];
    }

    void setModuleSyncState(SyncType type, bool state);

    QString userDisplayName() const;

    inline bool getActivation() const { return m_activation; }
    void setActivation(bool value);

    inline QString getUUID() const  { return m_uuID; }
    void setUUID(const QString &uuid);

    inline QString getUOSID() const { return m_uosID; }
    void setUOSID(const QString &uosid);

    inline QString getHostName() const { return m_hostName; }
    void setHostName(const QString &hostname);

    inline QString getUbID() const  { return m_ubID; }
    void setBindLocalUBid(const QString &bindUBid);

    void setResetPasswdError(const QString &errorInfo);
    void setResetUserNameError(const QString &errorInfo);

Q_SIGNALS:
    void userInfoChanged(const QVariantMap &userInfo);
    void syncStateChanged(const std::pair<qint32, QString>& syncState);
    void lastSyncTimeChanged(const qlonglong lastSyncTime);
    void enableSyncChanged(bool enableSync);
    void moduleSyncStateChanged(std::pair<SyncType, bool> state);
    void syncIsValidChanged(bool syncIsValid);
    void licenseStateChanged(bool activation);
    void userUUIDInfoChanged(const QString& uuid);
    void userUOSIDInfoChanged(const QString& uuid);
    void userHostNameInfoChanged(const QString& uuid);
    void userUnbindInfoChanged(const QString& ubid);
    void resetPasswdError(const QString& errorInfo);
    void resetUserNameError(const QString& errorInfo);

private:
    bool m_enableSync;
    bool m_activation;
    QMap<SyncType, bool> m_moduleSyncState;
    QVariantMap m_userinfo;
    std::pair<qint32, QString> m_syncState;
    qlonglong m_lastSyncTime;
    QString m_uosID;
    QString m_uuID;
    QString m_hostName;
    QString m_ubID;
    QString m_bindLocalUBid; // 绑定后的UBID(如果没有绑定，返回空字符串)
};

#endif  // SYNCMODEL_H
