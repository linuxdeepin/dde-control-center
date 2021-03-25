#ifndef UnionidModel_H
#define UnionidModel_H

#include "../sync/utils.h"

#include <QObject>
#include <QVariantMap>
#include <utility>

namespace dcc {
namespace unionid {
class UnionidModel : public QObject {
    Q_OBJECT
public:
    explicit UnionidModel(QObject *parent = nullptr);

    inline const QVariantMap userinfo() const { return m_userinfo; }
    void setUserinfo(const QVariantMap &userinfo);

    inline std::pair<qint32, QString> syncState() const { return m_syncState; }
    void setSyncState(const std::pair<qint32, QString> &syncState);

    static bool isSyncStateValid(const std::pair<qint32, QString> &state);
    static bool isSyncSucceed(const std::pair<qint32, QString> &state);
    static bool isSyncFailed(const std::pair<qint32, QString> &state);
    static bool isSyncing(const std::pair<qint32, QString> &state);

    inline bool enableSync() const { return m_enableSync; }
    void setEnableSync(bool enableSync);

    static std::list<std::pair<dcc::cloudsync::SyncType, QStringList>> moduleMap();

    inline QMap<dcc::cloudsync::SyncType, bool> moduleSyncState() const { return m_moduleSyncState; }
    inline bool getModuleStateByType(dcc::cloudsync::SyncType type) {
        return m_moduleSyncState[type];
    }

    void setModuleSyncState(dcc::cloudsync::SyncType type, bool state);

    inline bool syncIsValid() const { return /*m_syncIsValid*/true; }
    void setSyncIsValid(bool syncIsValid);
    QString userDisplayName() const;
    QString userEmail() const;

    void setActivation(bool value);
    inline bool getActivation() const { return m_activation; }


Q_SIGNALS:
    void userInfoChanged(const QVariantMap &userInfo);
    void syncStateChanged(const std::pair<qint32, QString>& syncState);
    void lastSyncTimeChanged(const qlonglong lastSyncTime);
    void enableSyncChanged(bool enableSync);
    void moduleSyncStateChanged(std::pair<dcc::cloudsync::SyncType, bool> state);
    void syncIsValidChanged(bool syncIsValid);
    void licenseStateChanged(bool activation);

private:
    bool m_syncIsValid;
    bool m_enableSync;
    bool m_activation;
    QMap<dcc::cloudsync::SyncType, bool> m_moduleSyncState;
    QVariantMap m_userinfo;
    std::pair<qint32, QString> m_syncState;
};
}  // namespace cloudsync
}  // namespace dcc

#endif  // UnionidModel_H
