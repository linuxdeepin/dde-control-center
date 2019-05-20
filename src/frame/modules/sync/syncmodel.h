#ifndef SYNCMODEL_H
#define SYNCMODEL_H

#include <QObject>
#include <QVariantMap>

namespace dcc {
namespace sync {
class SyncModel : public QObject {
    Q_OBJECT
public:
    explicit SyncModel(QObject *parent = nullptr);

    enum SyncType { Network, Sound, Mouse, Update, Dock, Launcher, Wallpaper };

    const QVariantMap userinfo() const { return m_userinfo; }
    void setUserinfo(const QVariantMap &userinfo);

Q_SIGNALS:
    void userInfoChanged(const QVariantMap &userInfo);

private:
    QVariantMap m_userinfo;
};
}  // namespace sync
}  // namespace dcc

#endif  // SYNCMODEL_H
