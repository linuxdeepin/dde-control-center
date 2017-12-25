/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
#include <com_deepin_daemon_network.h>
#include <com_deepin_lastoresessionhelper.h>

#include "common.h"

using UpdateInter=com::deepin::lastore::Updater;
using JobInter=com::deepin::lastore::Job;
using ManagerInter=com::deepin::lastore::Manager;
using PowerInter=com::deepin::daemon::Power;
using Network=com::deepin::daemon::Network;
using LastoressionHelper=com::deepin::LastoreSessionHelper;

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

    void setAutoCleanCache(const bool autoCleanCache);
    void setAutoDownloadUpdates(const bool &autoDownload);
    void setMirrorSource(const MirrorInfo &mirror);
    void setSourceCheck(bool enable);
    void testMirrorSpeed();

private slots:
    void setCheckUpdatesJob(const QString &jobPath);
    void setDownloadJob(const QString &jobPath);
    void setDistUpgradeJob(const QString &jobPath);
    void setOtherUpdate(const QString &jobPath);

    void onJobListChanged(const QList<QDBusObjectPath> &jobs);
    void onAppUpdateInfoFinished(QDBusPendingCallWatcher *w);
    void onDownloadStatusChanged(const QString &status);
    void onUpgradeStatusChanged(const QString &status);

    DownloadInfo *calculateDownloadInfo(const AppUpdateInfoList &list);

private:
    AppUpdateInfo getInfo(const AppUpdateInfo &packageInfo, const QString& currentVersion, const QString& lastVersion) const;
    AppUpdateInfo getDDEInfo();
    void distUpgradeDownloadUpdates();
    void distUpgradeInstallUpdates();
    void setAppUpdateInfo(const AppUpdateInfoList &list);

private:
    UpdateModel* m_model;
    JobInter* m_downloadJob;
    JobInter* m_checkUpdateJob;
    JobInter* m_distUpgradeJob;
    JobInter* m_otherUpdateJob;
    LastoressionHelper *m_lastoresessionHelper;
    UpdateInter* m_updateInter;
    ManagerInter* m_managerInter;
    PowerInter *m_powerInter;
    Network *m_networkInter;
    bool m_onBattery;
    double m_batteryPercentage;

    double m_baseProgress;

    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
};
}
}
#endif // UPDATEWORK_H
