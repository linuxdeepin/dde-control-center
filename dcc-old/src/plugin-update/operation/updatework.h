// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "common.h"
#include "updatedbusproxy.h"
#include "updatejobdbusproxy.h"
#include "updatemodel.h"

#include <QNetworkReply>
#include <QObject>
#include <DConfig>

#include <optional>

struct CheckUpdateJobRet
{
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
        "createdAt": "2023-08-10T17:45:54+08:00",
        "logType": 1,
        "publishTime": "2023-08-06T00:00:00+08:00"
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
    /**
     * @brief preInitialize  更新预加载 用于获取加载前更新红点状态
     */
    void preInitialize();
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

    void handleAtomicStateChanged(int operate, int state, QString version, QString message);
    void onAtomicUpdateFinshed(bool successed);
    void onAtomicUpdateing();

Q_SIGNALS:
    void requestInit();
    void requestActive();
    void requestRefreshLicenseState();
    void releaseNoteInstallCompleted();

public Q_SLOTS:
    void init();
    void checkForUpdates();
    void requestUpdateLog();
    void distUpgrade(ClassifyUpdateType updateType);
    void setAutoCheckUpdates(const bool autoCheckUpdates);
    void setUpdateMode(const quint64 updateMode);
    void setAutoCleanCache(const bool autoCleanCache);
    void setAutoDownloadUpdates(const bool &autoDownload);
    void setAutoInstallUpdates(const bool &autoInstall);
    void setBackupUpdates(const bool &backupUpdates);

    void handleUpdateLogsReply(QNetworkReply *reply);
    QString getUpdateLogAddress() const;
    int isUnstableResource() const;
    void setUpdateLogs(const QJsonArray &array);

    void checkNetselect();

    void licenseStateChangeSlot();
    void testingChannelChangeSlot();
    void refreshLastTimeAndCheckCircle();
    void setUpdateNotify(const bool notify);

    void OnDownloadJobCtrl(ClassifyUpdateType type, int updateCtrlType);
    void onRequestOpenAppStore();
    void onClassifiedUpdatablePackagesChanged(QMap<QString, QStringList> packages);
    void onFixError(const ClassifyUpdateType &updateType, const QString &errorType);
    void onRequestLastoreHeartBeat();

    std::optional<QUrl> updateTestingChannelUrl();
    std::optional<QUrl> getTestingChannelUrl();
    std::optional<QString> getMachineId();

    void setTestingChannelEnable(const bool &enable);
    void checkTestingChannelStatus();

    void refreshMirrors();
    void setSmartMirror(bool enable);
    void setMirrorSource(const MirrorInfo& mirror);
    void testMirrorSpeed();

private Q_SLOTS:
    void setCheckUpdatesJob(const QString &jobPath);
    void onJobListChanged(const QList<QDBusObjectPath> &jobs);
    void checkDiskSpace(const QString &jobDescription);

    void onCheckUpdateStatusChanged(const QString &value);
    void onClassityDownloadStatusChanged(const ClassifyUpdateType type, const QString &value);
    void onClassityInstallStatusChanged(const ClassifyUpdateType type, const QString &value);

    void onSysUpdateDownloadProgressChanged(double value);
    void onSafeUpdateDownloadProgressChanged(double value);
    void onUnkonwnUpdateDownloadProgressChanged(double value);

    void onSysUpdateInstallProgressChanged(double value);
    void onSafeUpdateInstallProgressChanged(double value);
    void onUnkonwnUpdateInstallProgressChanged(double value);

private:
    QMap<ClassifyUpdateType, UpdateItemInfo *> getAllUpdateInfo();
    void getItemInfo(QJsonValue jsonValue, UpdateItemInfo *itemInfo);
    void setUpdateInfo();
    void setUpdateItemDownloadSize(UpdateItemInfo *updateItem, QStringList packages);

    inline bool checkDbusIsValid();
    bool getNotUpdateState();
    void resetDownloadInfo(bool state = false);
    CheckUpdateJobRet createCheckUpdateJob(const QString &jobPath);

    void downloadAndInstallUpdates(ClassifyUpdateType updateType);

    void setDownloadJob(const QString &jobPath, ClassifyUpdateType updateType);
    void setDistUpgradeJob(const QString &jobPath, ClassifyUpdateType updateType);
    void setUpdateItemProgress(UpdateItemInfo *itemInfo, double value);

    QPointer<UpdateJobDBusProxy> getDownloadJob(ClassifyUpdateType updateType);
    QPointer<UpdateJobDBusProxy> getInstallJob(ClassifyUpdateType updateType);

    bool checkJobIsValid(QPointer<UpdateJobDBusProxy> dbusJob);
    void deleteJob(QPointer<UpdateJobDBusProxy> dbusJob);
    void deleteClassityDownloadJob(ClassifyUpdateType type);
    void deleteClassityInstallJob(ClassifyUpdateType type);
    bool checkUpdateSuccessed();
    void cleanLastoreJob(QPointer<UpdateJobDBusProxy> dbusJob);
    UpdateErrorType analyzeJobErrorMessage(QString jobDescription);
    QString getClassityUpdateDownloadJobName(ClassifyUpdateType updateType);
    void checkUpdatablePackages(const QMap<QString, QStringList> &updatablePackages);

    void backupToAtomicUpgrade();
    void updateItemInfo(const UpdateLogItem &logItem, UpdateItemInfo *itemInfo);

    // testingChannel
    CanExitTestingChannelStatus checkCanExitTestingChannelDialog();
    std::optional<QString> getTestingChannelSource();
    QStringList getSourcesOfPackage(const QString &pkg, const QString &version);

private:
    UpdateModel *m_model;
    QPointer<UpdateJobDBusProxy> m_checkUpdateJob;
    QPointer<UpdateJobDBusProxy> m_fixErrorJob;

    QPointer<UpdateJobDBusProxy> m_sysUpdateDownloadJob;
    QPointer<UpdateJobDBusProxy> m_safeUpdateDownloadJob;
    QPointer<UpdateJobDBusProxy> m_unknownUpdateDownloadJob;

    QPointer<UpdateJobDBusProxy> m_sysUpdateInstallJob;
    QPointer<UpdateJobDBusProxy> m_safeUpdateInstallJob;
    QPointer<UpdateJobDBusProxy> m_unknownUpdateInstallJob;

    QPointer<UpdateJobDBusProxy> m_releaseNoteInstallJob;

    QString m_sysUpdateDownloadJobName;
    QString m_safeUpdateDownloadJobName;
    QString m_unknownUpdateDownloadJobName;

    UpdateDBusProxy *m_updateInter;
    bool m_onBattery;
    double m_batteryPercentage;
    double m_batterySystemPercentage;
    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
    QString m_jobPath;
    qlonglong m_downloadSize;

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

    std::optional<QString> m_machineid;
    std::optional<QUrl> m_testingChannelUrl;

    bool m_isFirstActive;

    DConfig *m_updateConfig;
};

#endif // UPDATEWORK_H
