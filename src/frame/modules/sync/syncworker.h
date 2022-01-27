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

struct BindCheckResult {
    QString ubid = "";
    QString error = "";
    bool ret = false;
};

class SyncWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit SyncWorker(SyncModel * model, QObject *parent = nullptr);

    virtual void activate();
    virtual void deactivate();

    void refreshSyncState();
Q_SIGNALS:
    void ubid(const QString &ubid);
    void resetPasswdError(const QString &error);
    void unBindRet(bool);
public Q_SLOTS:
    void setSync(std::pair<SyncType, bool> state);
    void loginUser();
    void logoutUser();
    void asyncLogoutUser(const QString &ubid);
    void setAutoSync(bool autoSync);
    void licenseStateChangeSlot();
    void getUOSID(QString &uosid);
    void getUUID(QString &uuid);
    void getHostName(QString &hostName);
    void asyncLocalBindCheck(const QString &uosid, const QString &uuid);
    void asyncBindAccount(const QString &uuid, const QString &hostName);
    void asyncUnbindAccount(const QString &ubid);
private:
    void onSyncModuleStateChanged(const QString& module, bool enable);
    void onStateChanged(const IntString& state);
    void onLastSyncTimeChanged(qlonglong lastSyncTime);
    void getLicenseState();
    BindCheckResult logout(const QString &uuid);
    BindCheckResult checkLocalBind(const QString &uosid, const QString &uuid);
    BindCheckResult bindAccount(const QString &uuid, const QString &hostName);
    BindCheckResult unBindAccount(const QString &ubid);
private:
    SyncModel *m_model;
    SyncInter *m_syncInter;
    DeepinId *m_deepinId_inter;
    QDBusInterface *m_syncHelperInter;
};
}
}

#endif // SYNCWORKER_H
