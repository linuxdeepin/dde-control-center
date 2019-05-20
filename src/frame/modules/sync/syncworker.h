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
namespace sync {
class SyncWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit SyncWorker(SyncModel * model, QObject *parent = nullptr);

    virtual void activate();
    virtual void deactivate();

public Q_SLOTS:
    void setSync(SyncModel::SyncType type, bool enable);
    void loginUser();
    void logoutUser();

private:
    SyncModel *m_model;
    SyncInter *m_syncInter;
    DeepinId *m_deepinId_inter;
};
}
}

#endif // SYNCWORKER_H
