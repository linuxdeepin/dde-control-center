// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SYNCMODEL_H
#define SYNCMODEL_H

#include "utils.h"

#include <QObject>
#include <QVariantMap>
#include <utility>

namespace dcc {
namespace cloudsync {
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

    inline bool syncIsValid() const { return m_syncIsValid; }
    void setSyncIsValid(bool syncIsValid);
    QString userDisplayName() const;

    inline bool getActivation() const { return m_activation; }
    void setActivation(bool value);

Q_SIGNALS:
    void userInfoChanged(const QVariantMap &userInfo);
    void syncStateChanged(const std::pair<qint32, QString>& syncState);
    void lastSyncTimeChanged(const qlonglong lastSyncTime);
    void enableSyncChanged(bool enableSync);
    void moduleSyncStateChanged(std::pair<SyncType, bool> state);
    void syncIsValidChanged(bool syncIsValid);
    void licenseStateChanged(bool activation);

private:
    bool m_syncIsValid;
    bool m_enableSync;
    bool m_activation;
    QMap<SyncType, bool> m_moduleSyncState;
    QVariantMap m_userinfo;
    std::pair<qint32, QString> m_syncState;
    qlonglong m_lastSyncTime;
};
}  // namespace cloudsync
}  // namespace dcc

#endif  // SYNCMODEL_H
