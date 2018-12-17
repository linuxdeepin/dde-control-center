#ifndef SYNCMODEL_H
#define SYNCMODEL_H

#include <QObject>

namespace dcc {
namespace sync {
class SyncModel : public QObject {
    Q_OBJECT
public:
    explicit SyncModel(QObject *parent = nullptr);

    enum SyncType { Network, Sound, Mouse, Update, Dock, Launcher, Wallpaper };

Q_SIGNALS:

public Q_SLOTS:
};
}  // namespace sync
}  // namespace dcc

#endif  // SYNCMODEL_H
