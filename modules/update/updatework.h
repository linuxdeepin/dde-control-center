#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "updatemodel.h"

#include <QObject>
#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>
#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_daemon_power.h>

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
    MirrorInfoList mirrorInfos() const;
    QString defaultMirror() const;
    void setMirrorSource(const QString& src);

    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);

    void prepareDistUpgrade();
    bool pauseJob();
    bool startJob();

signals:
    void mirrorSourceChanged(const QString& src);
    void autoCheckUpdatesChanged(bool update);
    void appInfos(const QList<AppUpdateInfo>& infos);
    void packageDownloadSize(qlonglong size, int count);
    void progressChanged(double progress);
    void statusChanged(bool useBattery, double percent);

public slots:
    void onStatus(const QString& status);
    void checkUpdate();
    void onCheckUpdateStatus(const QString& status);
    void onPackagesDownloadSize(QDBusPendingCallWatcher* watcher);

private:
    AppUpdateInfo getInfo(const QString& packageName, const QString& currentVersion, const QString& lastVersion) const;
    QList<AppUpdateInfo> getInfoList();
    void loadAppList();
    QStringList updatableApps() const;
    QStringList updatablePackages() const;

    void loadDownloadJob(JobInter* newJob);

private:
    UpdateModel* m_model;
    UpdateInter* m_updateInter;
    JobInter* m_downloadJob;
    JobInter* m_checkUpdateJob;
    ManagerInter* m_managerInter;

    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
};
}
}
#endif // UPDATEWORK_H
