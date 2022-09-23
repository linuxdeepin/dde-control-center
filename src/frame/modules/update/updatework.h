// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "updatemodel.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>
#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_daemon_power.h>
#include <com_deepin_system_systempower.h>
#include <com_deepin_daemon_network.h>
#include <com_deepin_lastoresessionhelper.h>
#include <com_deepin_lastore_smartmirror.h>
#include <com_deepin_abrecovery.h>
#include <com_deepin_daemon_appearance.h>

#include "common.h"

using UpdateInter = com::deepin::lastore::Updater;
using JobInter = com::deepin::lastore::Job;
using ManagerInter = com::deepin::lastore::Manager;
using PowerInter = com::deepin::daemon::Power;
using PowerSystemInter = com::deepin::system::Power;
using Network = com::deepin::daemon::Network;
using LastoressionHelper = com::deepin::LastoreSessionHelper;
using SmartMirrorInter = com::deepin::lastore::Smartmirror;
using RecoveryInter = com::deepin::ABRecovery;
using Appearance = com::deepin::daemon::Appearance;

class QJsonArray;

namespace dcc {
namespace update {

struct CheckUpdateJobRet {
    QString status;
    QString jobID;
    QString jobDescription;
};

/**
 * @brief 更新日志中一个版本的信息
 *
 * 示例数据：
 * {
        "id": 1,
        "platformType": 1,
        "cnLog": "<p>中文日志</p>",
        "enLog": "<p>英文日志</p>",
        "serverType": 0,
        "systemVersion": "1070U1",
        "createdAt": "2022-08-10T17:45:54+08:00",
        "logType": 1,
        "publishTime": "2022-08-06T00:00:00+08:00"
    }
 */
struct UpdateLogItem
{
    int id = -1;
    int platformType = 1;
    int serverType = 0;
    int logType = 1;
    QString systemVersion = "";
    QString cnLog = "";
    QString enLog = "";
    QString publishTime = "";

    bool isValid() const { return -1 != id; }
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

    void setSysUpdateDownloadJobName(const QString &sysUpdateDownloadJobName);
    void setSafeUpdateDownloadJobName(const QString &safeUpdateDownloadJobName);
    void setUnknownUpdateDownloadJobName(const QString &unknownUpdateDownloadJobName);

    bool hasRepositoriesUpdates();

Q_SIGNALS:
    void requestInit();
    void requestActive();
    void requestRefreshLicenseState();

#ifndef DISABLE_SYS_UPDATE_MIRRORS
    void requestRefreshMirrors();
#endif

public Q_SLOTS:
    void init();
    void checkForUpdates();
    void distUpgrade(ClassifyUpdateType updateType);
    void setAutoCheckUpdates(const bool autoCheckUpdates);
    void setUpdateMode(const quint64 updateMode);
    void setAutoCleanCache(const bool autoCleanCache);
    void setAutoDownloadUpdates(const bool &autoDownload);
    void setAutoInstallUpdates(const bool &autoInstall);
    void setMirrorSource(const MirrorInfo &mirror);
    void setTestingChannelEnable(const bool &enable);
    void checkCanExitTestingChannel();
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void setSourceCheck(bool enable);
#endif
    void testMirrorSpeed();
    void checkNetselect();
    void setSmartMirror(bool enable);
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    void refreshMirrors();
#endif
    void recoveryCanBackup(ClassifyUpdateType type);
    void recoveryStartRestore();
    void onRecoveryBackupFinshed(const QString &kind, const bool success, const QString &errMsg);

    void licenseStateChangeSlot();
    void refreshHistoryAppsInfo();
    void refreshLastTimeAndCheckCircle();
    void setUpdateNotify(const bool notify);

    void OnDownloadJobCtrl(ClassifyUpdateType type, int updateCtrlType);
    void onRequestOpenAppStore();
    void onClassifiedUpdatablePackagesChanged(QMap<QString, QStringList> packages);
    void onFixError(const ClassifyUpdateType &updateType, const QString &errorType);
    void onRequestLastoreHeartBeat();

private Q_SLOTS:
    void setCheckUpdatesJob(const QString &jobPath);
    void onJobListChanged(const QList<QDBusObjectPath> &jobs);
    void checkDiskSpace(const QString &jobDescription);
    void onIconThemeChanged(const QString &theme);

    void onCheckUpdateStatusChanged(const QString &value);
    void onClassityDownloadStatusChanged(const ClassifyUpdateType type, const QString &value);
    void onClassityInstallStatusChanged(const ClassifyUpdateType type, const QString &value);

    void onSysUpdateDownloadProgressChanged(double value);
    void onSafeUpdateDownloadProgressChanged(double value);
    void onUnkonwnUpdateDownloadProgressChanged(double value);

    void onSysUpdateInstallProgressChanged(double value);
    void onSafeUpdateInstallProgressChanged(double value);
    void onUnkonwnUpdateInstallProgressChanged(double value);
    void checkTestingChannelStatus();
    QStringList getSourcesOfPackage(const QString pkg, const QString version);
    QString getTestingChannelSource();
    void handleUpdateLogsReply(QNetworkReply *reply);
    QString getUpdateLogAddress() const;

private:
    QMap<ClassifyUpdateType, UpdateItemInfo *> getAllUpdateInfo();
    void setUpdateInfo();
    void setUpdateItemDownloadSize(UpdateItemInfo *updateItem, QStringList packages);

    inline bool checkDbusIsValid();
    void onSmartMirrorServiceIsValid(bool isvalid);
    bool getNotUpdateState();
    void resetDownloadInfo(bool state = false);
    CheckUpdateJobRet createCheckUpdateJob(const QString &jobPath);

    void downloadAndInstallUpdates(ClassifyUpdateType updateType);

    void setDownloadJob(const QString &jobPath, ClassifyUpdateType updateType);
    void setDistUpgradeJob(const QString &jobPath, ClassifyUpdateType updateType);
    void setUpdateItemProgress(UpdateItemInfo *itemInfo, double value);
    bool hasBackedUp();
    void onRecoveryFinshed(bool successed);

    QPointer<JobInter> getDownloadJob(ClassifyUpdateType updateType);
    QPointer<JobInter> getInstallJob(ClassifyUpdateType updateType);

    bool checkJobIsValid(QPointer<JobInter> dbusJob);
    void deleteJob(QPointer<JobInter> dbusJob);
    void deleteClassityDownloadJob(ClassifyUpdateType type);
    void deleteClassityInstallJob(ClassifyUpdateType type);
    bool checkUpdateSuccessed();
    void cleanLastoreJob(QPointer<JobInter> dbusJob);
    UpdateErrorType analyzeJobErrorMessage(QString jobDescription);
    QString getClassityUpdateDownloadJobName(ClassifyUpdateType updateType);
    void checkUpdatablePackages(const QMap<QString, QStringList> &updatablePackages);
    void requestUpdateLog();
    void updateItemInfo(const UpdateLogItem &logItem, UpdateItemInfo *itemInfo);
    void setUpdateLogs(const QJsonArray &array);
    int isUnstableResource() const;

private:
    UpdateModel *m_model;
    QPointer<JobInter> m_checkUpdateJob;
    QPointer<JobInter> m_fixErrorJob;

    QPointer<JobInter> m_sysUpdateDownloadJob;
    QPointer<JobInter> m_safeUpdateDownloadJob;
    QPointer<JobInter> m_unknownUpdateDownloadJob;

    QPointer<JobInter> m_sysUpdateInstallJob;
    QPointer<JobInter> m_safeUpdateInstallJob;
    QPointer<JobInter> m_unknownUpdateInstallJob;

    QString m_sysUpdateDownloadJobName;
    QString m_safeUpdateDownloadJobName;
    QString m_unknownUpdateDownloadJobName;

    LastoressionHelper *m_lastoresessionHelper;
    UpdateInter *m_updateInter;
    ManagerInter *m_managerInter;
    PowerInter *m_powerInter;
    PowerSystemInter *m_powerSystemInter;
    Network *m_networkInter;
    SmartMirrorInter *m_smartMirrorInter;
    RecoveryInter *m_abRecoveryInter;
    Appearance *m_iconTheme;
    bool m_onBattery;
    double m_batteryPercentage;
    double m_batterySystemPercentage;
    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
    QString m_jobPath;
    qlonglong m_downloadSize;
    QString m_iconThemeState;

    QMap<QString, QStringList> m_updatePackages;
    QStringList m_systemPackages;
    QStringList m_safePackages;
    QStringList m_unknownPackages;

    // 当前备份状态
    BackupStatus m_backupStatus;
    // 当前正在备份的更新分类类型
    ClassifyUpdateType m_backupingClassifyType;
    QList<ClassifyUpdateType> m_fixErrorUpdate;

    QMutex m_mutex;
    QMutex m_downloadMutex;
    QList<UpdateLogItem> m_updateLogs;
};

}
}
#endif // UPDATEWORK_H
