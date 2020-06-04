/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "updatemodel.h"

#include <QObject>
#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>
#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_daemon_power.h>
#include <com_deepin_system_systempower.h>
#include <com_deepin_daemon_network.h>
#include <com_deepin_lastoresessionhelper.h>
#include <com_deepin_lastore_smartmirror.h>
#include <com_deepin_abrecovery.h>
#include <com_deepin_daemon_systeminfo.h>
#include <com_deepin_daemon_appearance.h>

#include "common.h"

using UpdateInter=com::deepin::lastore::Updater;
using JobInter=com::deepin::lastore::Job;
using ManagerInter=com::deepin::lastore::Manager;
using PowerInter=com::deepin::daemon::Power;
using PowerSystemInter=com::deepin::system::Power;
using Network=com::deepin::daemon::Network;
using LastoressionHelper=com::deepin::LastoreSessionHelper;
using SmartMirrorInter = com::deepin::lastore::Smartmirror;
using RecoveryInter = com::deepin::ABRecovery;
using SystemInfoInter=com::deepin::daemon::SystemInfo;
using Appearance = com::deepin::daemon::Appearance;
namespace dcc{
namespace update{

struct CheckUpdateJobRet {
    QString status;
    QString jobID;
    QString jobDescription;
};

class UpdateWorker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateWorker(UpdateModel *model, QObject *parent = nullptr);
    ~UpdateWorker();
    void activate();
    void deactivate();
    void setOnBattery(bool onBattery);
    void setBatteryPercentage(const BatteryPercentageInfo &info);
    void setSystemBatteryPercentage(const double &value);
    void getLicenseState();

public Q_SLOTS:
    void checkForUpdates();
    void pauseDownload();
    void resumeDownload();
    void distUpgrade();
    void downloadAndDistUpgrade();
    void setAutoCheckUpdates(const bool autocheckUpdates);
    void setAutoCleanCache(const bool autoCleanCache);
    void setAutoDownloadUpdates(const bool &autoDownload);
    void setMirrorSource(const MirrorInfo &mirror);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void setSourceCheck(bool enable);
#endif
    void testMirrorSpeed();
    void checkNetselect();
    void setSmartMirror(bool enable);
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    void refreshMirrors();
#endif
    void recoveryCanBackup();
    void recoveryStartRestore();
    void onNotifyDownloadInfoChanged();
    void licenseStateChangeSlot();

    void refreshHistoryAppsInfo();
    void refreshLastTimeAndCheckCircle();
    void setUpdateNotify(const bool notify);

private Q_SLOTS:
    void setCheckUpdatesJob(const QString &jobPath);
    void setDownloadJob(const QString &jobPath);
    void setDistUpgradeJob(const QString &jobPath);
    void onJobListChanged(const QList<QDBusObjectPath> &jobs);
    void onAppUpdateInfoFinished(QDBusPendingCallWatcher *w);
    void onDownloadStatusChanged(const QString &status);
    void onUpgradeStatusChanged(const QString &status);
    void checkDiskSpace(const QString &jobDescription);
    DownloadInfo *calculateDownloadInfo(const AppUpdateInfoList &list);
    void onIconThemeChanged(const QString &theme);

private:
    AppUpdateInfo getInfo(const AppUpdateInfo &packageInfo, const QString& currentVersion, const QString& lastVersion) const;
    AppUpdateInfo getDDEInfo();
    void distUpgradeDownloadUpdates();
    void distUpgradeInstallUpdates();
    void setAppUpdateInfo(const AppUpdateInfoList &list);
    inline bool checkDbusIsValid();
    void onSmartMirrorServiceIsValid(bool isvalid);
    void onNotifyStatusChanged(UpdatesStatus status);
    bool getNotUpdateState();
    void resetDownloadInfo(bool state = false);
    CheckUpdateJobRet createCheckUpdateJob(const QString &jobPath);

private:
    UpdateModel* m_model;
    QPointer<JobInter> m_downloadJob;
    QPointer<JobInter> m_checkUpdateJob;
    QPointer<JobInter> m_distUpgradeJob;
    QPointer<JobInter> m_otherUpdateJob;
    LastoressionHelper *m_lastoresessionHelper;
    UpdateInter* m_updateInter;
    ManagerInter* m_managerInter;
    PowerInter *m_powerInter;
    PowerSystemInter *m_powerSystemInter;
    Network *m_networkInter;
    SmartMirrorInter *m_smartMirrorInter;
    RecoveryInter *m_abRecoveryInter;
    SystemInfoInter *m_systemInfoInter;
    Appearance *m_iconTheme;
    bool m_onBattery;
    double m_batteryPercentage;
    double m_batterySystemPercentage;
    double m_baseProgress;
    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
    bool m_bDownAndUpdate;
    QString m_jobPath;
    double m_downloadProcess;
    bool m_bIsFirstGetDownloadProcess;
    qlonglong m_downloadSize;
    bool m_beginUpdatesJob;
    QString m_iconThemeState;
};
}
}
#endif // UPDATEWORK_H
