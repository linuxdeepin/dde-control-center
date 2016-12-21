#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "updatemodel.h"

#include <QObject>
#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>
#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_daemon_power.h>

#include "common.h"

using UpdateInter=com::deepin::lastore::Updater;
using JobInter=com::deepin::lastore::Job;
using ManagerInter=com::deepin::lastore::Manager;
using PowerInter=com::deepin::daemon::Power;

namespace dcc{
namespace update{
class UpdateWork : public QObject
{
    Q_OBJECT

public:
    explicit UpdateWork(UpdateModel *model, QObject *parent = 0);

    void activate();
    void deactivate();

public slots:
    void checkForUpdates();
    void downloadUpdates();
    void pauseDownload();
    void resumeDownload();

    void setAutoUpdate(const bool &autoUpdate);
    void setMirrorSource(const MirrorInfo &mirror);

    void testMirrorSpeed();

private slots:
    void setCheckUpdatesJob(const QString &jobPath);
    void setDownloadJob(const QString &jobPath);

    DownloadInfo *calculateDownloadInfo();

private:
    AppUpdateInfo getInfo(const QString& packageName, const QString& currentVersion, const QString& lastVersion) const;
    QList<AppUpdateInfo> getInfoList();

private:
    UpdateModel* m_model;
    JobInter* m_downloadJob;
    JobInter* m_checkUpdateJob;
    UpdateInter* m_updateInter;
    ManagerInter* m_managerInter;
    PowerInter *m_powerInter;

    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
};
}
}
#endif // UPDATEWORK_H
