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

    void setOnBattery(bool onBattery);
    void setBatteryPercentage(const BatteryPercentageInfo &info);

public slots:
    void checkForUpdates();
    void pauseDownload();
    void resumeDownload();
    void distUpgrade();
    void downloadAndDistUpgrade();

    void setAutoDownloadUpdates(const bool &autoDownload);
    void setMirrorSource(const MirrorInfo &mirror);

    void testMirrorSpeed();

private slots:
    void setCheckUpdatesJob(const QString &jobPath);
    void setDownloadJob(const QString &jobPath);
    void setDistUpgradeJob(const QString &jobPath);

    void onAppUpdateInfoFinished(QDBusPendingCallWatcher *w);

    DownloadInfo *calculateDownloadInfo(const AppUpdateInfoList &list);

private:
    AppUpdateInfo getInfo(const AppUpdateInfo &packageInfo, const QString& currentVersion, const QString& lastVersion) const;

    void distUpgradeDownloadUpdates();
    void distUpgradeInstallUpdates();

private:
    UpdateModel* m_model;
    JobInter* m_downloadJob;
    JobInter* m_checkUpdateJob;
    JobInter* m_distUpgradeJob;
    UpdateInter* m_updateInter;
    ManagerInter* m_managerInter;
    PowerInter *m_powerInter;

    bool m_onBattery;
    double m_batteryPercentage;

    double m_baseProgress;

    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
};
}
}
#endif // UPDATEWORK_H
