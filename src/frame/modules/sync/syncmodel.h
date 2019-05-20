#ifndef SYNCMODEL_H
#define SYNCMODEL_H

#include <QObject>
#include <QVariantMap>
#include <utility>

namespace dcc {
namespace sync {
class SyncModel : public QObject {
    Q_OBJECT
public:
    explicit SyncModel(QObject *parent = nullptr);

    enum SyncType { Network, Sound, Mouse, Update, Dock, Launcher, Wallpaper };

    enum SyncState {
        Succeed,
        Syncing,
        Failed,
    };

    inline const QVariantMap userinfo() const { return m_userinfo; }
    void setUserinfo(const QVariantMap &userinfo);

    inline std::pair<qint32, QString> syncState() const { return m_syncState; }
    void setSyncState(const std::pair<qint32, QString> &syncState);

    static bool isSyncSucceed(const std::pair<qint32, QString> &state);
    static bool isSyncFailed(const std::pair<qint32, QString> &state);
    static bool isSyncing(const std::pair<qint32, QString> &state);

    inline qlonglong lastSyncTime() const { return m_lastSyncTime; }
    void setLastSyncTime(const qlonglong &lastSyncTime);

Q_SIGNALS:
    void userInfoChanged(const QVariantMap &userInfo);
    void syncStateChanged(const std::pair<qint32, QString>& syncState);
    void lastSyncTimeChanged(const qlonglong lastSyncTime);

private:
    QVariantMap m_userinfo;
    std::pair<qint32, QString> m_syncState;
    qlonglong m_lastSyncTime;
};
}  // namespace sync
}  // namespace dcc

#endif  // SYNCMODEL_H
