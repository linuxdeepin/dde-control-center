#ifndef SYNCWORKER_H
#define SYNCWORKER_H

#include "modules/moduleworker.h"
#include "syncmodel.h"

#include <QObject>
#include <com_deepin_sync_daemon.h>
#include <com_deepin_deepinid.h>

using SyncInter = com::deepin::sync::Daemon;
using DeepinId = com::deepin::deepinid;

namespace dcc {
namespace cloudsync {
class SyncWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit SyncWorker(SyncModel * model, QObject *parent = nullptr);

    virtual void activate();
    virtual void deactivate();

    void refreshSyncState();

public Q_SLOTS:
    void setSync(std::pair<SyncModel::SyncType, bool> state);
    void loginUser();
    void logoutUser();
    void setAutoSync(bool autoSync);

private:
    void onSyncModuleStateChanged(const QString& module, bool enable);
    void onStateChanged(const IntString& state);
    void onGetModuleSyncStateFinished(QDBusPendingCallWatcher* watcher);
    void onLastSyncTimeChanged(qlonglong lastSyncTime);

private:
    SyncModel *m_model;
    SyncInter *m_syncInter;
    DeepinId *m_deepinId_inter;
};
}
}

#endif // SYNCWORKER_H
