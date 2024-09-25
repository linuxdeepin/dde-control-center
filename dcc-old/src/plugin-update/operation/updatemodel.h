// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEMODEL_H
#define UPDATEMODEL_H

#include "appupdateinfolist.h"
#include "common.h"
#include "mirrorinfolist.h"
#include "widgets/updateiteminfo.h"
#include "widgets/utils.h"

#include <QObject>

class DownloadInfo : public QObject
{
    Q_OBJECT
public:
    explicit DownloadInfo(const qlonglong &downloadSize,
                          const QList<AppUpdateInfo> &appInfos,
                          QObject *parent = 0);

    virtual ~DownloadInfo() { }

    inline qulonglong downloadSize() const { return m_downloadSize; }

    double downloadProgress() const { return m_downloadProgress; }

    QList<AppUpdateInfo> appInfos() const { return m_appInfos; }

    void setDownloadProgress(double downloadProgress);

Q_SIGNALS:
    void downloadProgressChanged(const double &progress);

private:
    qlonglong m_downloadSize;
    double m_downloadProgress;
    QList<AppUpdateInfo> m_appInfos;
};

struct UpdateJobErrorMessage
{
    QString jobErrorType;
    QString jobErrorMessage;
};

class UpdateModel : public QObject
{
    Q_OBJECT
public:
    explicit UpdateModel(QObject *parent = nullptr);
    ~UpdateModel();

public:
    // ModelUpdatesStatus仅用于log显示；
    // common.h UpdatesStatu更新后，此处需要同步更新
    enum ModelUpdatesStatus {
        Default,
        Checking,
        Updated,
        UpdatesAvailable,
        Updating,
        Downloading,
        DownloadPaused,
        Downloaded,
        AutoDownloaded,
        Installing,
        UpdateSucceeded,
        UpdateFailed,
        NeedRestart,
        WaitForRecoveryBackup,
        RecoveryBackingup,
        RecoveryBackingSuccessed,
        RecoveryBackupFailed,
        Inactive
    };
    Q_ENUM(ModelUpdatesStatus)

    UpdatesStatus status() const;
    void setStatus(const UpdatesStatus &status);
    void setStatus(const UpdatesStatus &status, int line);

    DownloadInfo *downloadInfo() const;
    void setDownloadInfo(DownloadInfo *downloadInfo);

    UpdateItemInfo *systemDownloadInfo() const;
    void setSystemDownloadInfo(UpdateItemInfo *updateItemInfo);

    UpdateItemInfo *safeDownloadInfo() const;
    void setSafeDownloadInfo(UpdateItemInfo *updateItemInfo);

    UpdateItemInfo *unknownDownloadInfo() const;
    void setUnknownDownloadInfo(UpdateItemInfo *updateItemInfo);

    QMap<ClassifyUpdateType, UpdateItemInfo *> allDownloadInfo() const;
    void setAllDownloadInfo(QMap<ClassifyUpdateType, UpdateItemInfo *> &allUpdateItemInfo);

    bool lowBattery() const;
    void setLowBattery(bool lowBattery);

    bool autoDownloadUpdates() const;
    void setAutoDownloadUpdates(bool autoDownloadUpdates);

    double upgradeProgress() const;
    void setUpgradeProgress(double upgradeProgress);

    bool autoCleanCache() const;
    void setAutoCleanCache(bool autoCleanCache);

    double updateProgress() const;
    void setUpdateProgress(double updateProgress);

    bool netselectExist() const;
    void setNetselectExist(bool netselectExist);

    inline bool autoCheckUpdates() const { return m_autoCheckUpdates; }

    void setAutoCheckUpdates(bool autoCheckUpdates);

    inline qulonglong updateMode() const { return m_updateMode; }

    void setUpdateMode(qulonglong updateMode);

    inline bool autoCheckSecureUpdates() const { return m_autoCheckSecureUpdates; }

    void setAutoCheckSecureUpdates(bool autoCheckSecureUpdates);

    inline bool autoCheckSystemUpdates() const { return m_autoCheckSystemUpdates; }

    void setAutoCheckSystemUpdates(bool autoCheckSystemUpdates);

    inline bool autoCheckAppUpdates() const { return m_autoCheckAppUpdates; }

    void setAutoCheckAppUpdates(bool autoCheckAppUpdates);

    inline QString systemVersionInfo() const { return m_systemVersionInfo; }

    void setSystemVersionInfo(const QString &systemVersionInfo);

    bool getIsRecoveryBackingup(UpdatesStatus state) const;

    inline UiActiveState systemActivation() const { return m_bSystemActivation; }

    void setSystemActivation(const UiActiveState &systemactivation);

    inline bool getUpdatablePackages() const { return m_isUpdatablePackages; }

    void isUpdatablePackages(bool isUpdatablePackages);

    const QString &lastCheckUpdateTime() const { return m_lastCheckUpdateTime; }

    void setLastCheckUpdateTime(const QString &lastTime);

    const QList<AppUpdateInfo> &historyAppInfos() const { return m_historyAppInfos; }

    void setHistoryAppInfos(const QList<AppUpdateInfo> &infos);

    int autoCheckUpdateCircle() const { return m_autoCheckUpdateCircle; }

    void setAutoCheckUpdateCircle(const int interval);
    bool enterCheckUpdate();

    inline bool updateNotify() { return m_updateNotify; }

    void setUpdateNotify(const bool notify);

    UpdatesStatus getSystemUpdateStatus() const;
    void setSystemUpdateStatus(const UpdatesStatus &systemUpdateStatus);

    UpdatesStatus getSafeUpdateStatus() const;
    void setSafeUpdateStatus(const UpdatesStatus &safeUpdateStatus);

    UpdatesStatus getUnkonowUpdateStatus() const;
    void setUnkonowUpdateStatus(const UpdatesStatus &unkonowUpdateStatus);

    void setClassifyUpdateTypeStatus(ClassifyUpdateType type, UpdatesStatus status);
    void setAllClassifyUpdateStatus(UpdatesStatus status);

    void deleteUpdateInfo(UpdateItemInfo *updateItemInfo);
    UpdatesStatus getClassifyUpdateStatus(ClassifyUpdateType type);

    bool getAutoInstallUpdates() const;
    void setAutoInstallUpdates(bool autoInstallUpdates);

    bool getBackupUpdates() const;
    void setBackupUpdates(bool backupsUpdates);

    quint64 getAutoInstallUpdateType() const;
    void setAutoInstallUpdateType(const quint64 &autoInstallUpdateType);

    QMap<ClassifyUpdateType, UpdateItemInfo *> getAllUpdateInfos() const;
    void setAllUpdateInfos(const QMap<ClassifyUpdateType, UpdateItemInfo *> &allUpdateInfos);

    UpdateJobErrorMessage getSystemUpdateJobError() const;
    void setSystemUpdateJobError(const UpdateJobErrorMessage &systemUpdateJobError);

    UpdateJobErrorMessage getSafeUpdateJobError() const;
    void setSafeUpdateJobError(const UpdateJobErrorMessage &safeUpdateJobError);

    UpdateJobErrorMessage getUnkonwUpdateJobError() const;
    void setUnkonwUpdateJobError(const UpdateJobErrorMessage &UnkonwUpdateJobError);

    void setClassityUpdateJonError(ClassifyUpdateType type, UpdateErrorType errorType);

    QMap<ClassifyUpdateType, UpdateErrorType> getUpdateErrorTypeMap() const;

    bool getAutoCheckThirdpartyUpdates() const;
    void setAutoCheckThirdpartyUpdates(bool autoCheckThirdpartyUpdates);

    // TODO : commit date update
    QString commitSubmissionTime();
    QString systemVersion();
    int32_t submissionType();
    QString UUID();

    QString utcDateTime2LocalDate(const QString &utcDateTime);
    // Testing Channel
    TestingChannelStatus getTestingChannelStatus() const;
    void setTestingChannelStatus(const TestingChannelStatus &status);
    // 智能镜像源
    void setMirrorInfos(const MirrorInfoList& list);
    MirrorInfoList mirrorInfos() const { return m_mirrorList; }

    MirrorInfo defaultMirror() const;
    void setDefaultMirror(const QString& mirrorId);

    bool smartMirrorSwitch() const { return m_smartMirrorSwitch; }
    void setSmartMirrorSwitch(bool smartMirrorSwitch);

    QMap<QString, int> mirrorSpeedInfo() const { return m_mirrorSpeedInfo; }
    void setMirrorSpeedInfo(const QMap<QString, int>& mirrorSpeedInfo);

Q_SIGNALS:
    void autoDownloadUpdatesChanged(const bool &autoDownloadUpdates);
    void autoInstallUpdatesChanged(const bool &autoInstallUpdates);
    void autoInstallUpdateTypeChanged(const quint64 &autoInstallUpdateType);

    void backupUpdatesChanged(const bool &backupUpdates);

    void lowBatteryChanged(const bool &lowBattery);
    void statusChanged(const UpdatesStatus &status);

    void systemUpdateStatusChanged(const UpdatesStatus &status);
    void safeUpdateStatusChanged(const UpdatesStatus &status);
    void unkonowUpdateStatusChanged(const UpdatesStatus &status);

    void systemUpdateDownloadSizeChanged(const qlonglong updateSize);
    void safeUpdateDownloadSizeChanged(const qlonglong updateSize);
    void unkonowUpdateDownloadSizeChanged(const qlonglong updateSize);

    void mirrorSpeedInfoAvaiable(const QMap<QString, int> &mirrorSpeedInfo);

    void downloadInfoChanged(DownloadInfo *downloadInfo);

    void systemUpdateInfoChanged(UpdateItemInfo *updateItemInfo);
    void safeUpdateInfoChanged(UpdateItemInfo *updateItemInfo);
    void unknownUpdateInfoChanged(UpdateItemInfo *updateItemInfo);

    void classityUpdateJobErrorChanged(const ClassifyUpdateType &type,
                                       const UpdateErrorType &errorType);

    void systemUpdateProgressChanged(const double &updateProgress);
    void safeUpdateProgressChanged(const double &updateProgress);
    void unkonowUpdateProgressChanged(const double &updateProgress);

    void updateProgressChanged(const double &updateProgress);
    void upgradeProgressChanged(const double &upgradeProgress);
    void autoCleanCacheChanged(const bool autoCleanCache);
    void netselectExistChanged(const bool netselectExist);
    void autoCheckUpdatesChanged(const bool autoCheckUpdates);
    void autoCheckSystemUpdatesChanged(const bool autoCheckSystemUpdate);
    void autoCheckAppUpdatesChanged(const bool autoCheckAppUpdate);
    void autoCheckSecureUpdatesChanged(const bool autoCheckSecureUpdate);
    void autoCheckThirdpartyUpdatesChanged(const bool autoCheckThirdpartyUpdate);
    void recoverRestoringChanged(bool recoverRestoring);
    void systemVersionChanged(QString version);
    void systemActivationChanged(UiActiveState systemactivation);
    void beginCheckUpdate();
    void updateCheckUpdateTime();
    void updateNotifyChanged(const bool notify);
    void updatablePackagesChanged(const bool isUpdatablePackages);

    // Testing Channel
    void TestingChannelStatusChanged(const TestingChannelStatus &status);

    void longlongAutoUpdateChanged(const bool longlongAutoUpdate);

    void defaultMirrorChanged(const MirrorInfo& mirror);
    void smartMirrorSwitchChanged(bool smartMirrorSwitch);
    void mirrorSpeedInfoAvailable(const QMap<QString, int>& mirrorSpeedInfo);

private:
    UpdatesStatus m_status;

    UpdatesStatus m_systemUpdateStatus;
    UpdatesStatus m_safeUpdateStatus;
    UpdatesStatus m_unkonowUpdateStatus;

    DownloadInfo *m_downloadInfo;

    QMap<ClassifyUpdateType, UpdateItemInfo *> m_allUpdateInfos;
    UpdateItemInfo *m_systemUpdateInfo;
    UpdateItemInfo *m_safeUpdateInfo;
    UpdateItemInfo *m_unknownUpdateInfo;

    double m_updateProgress;
    double m_upgradeProgress;

    bool m_lowBattery;
    bool m_netselectExist;
    bool m_autoCleanCache;
    bool m_autoDownloadUpdates;
    bool m_autoInstallUpdates;
    quint64 m_autoInstallUpdateType;
    bool m_backupUpdates;
    bool m_autoCheckUpdates;
    qulonglong m_updateMode;
    bool m_autoCheckSecureUpdates;
    bool m_autoCheckSystemUpdates;
    bool m_autoCheckAppUpdates;
    bool m_autoCheckThirdpartyUpdates;
    bool m_updateNotify;
    QString m_mirrorId;
    QMap<QString, int> m_mirrorSpeedInfo;

    QString m_systemVersionInfo;
    QMetaEnum m_metaEnum;
    UiActiveState m_bSystemActivation;

    QString m_lastCheckUpdateTime;          // 上次检查更新时间
    QList<AppUpdateInfo> m_historyAppInfos; // 历史更新应用列表
    int m_autoCheckUpdateCircle; // 决定进入检查更新界面是否自动检查,单位：小时
    bool m_isUpdatablePackages;

    QMap<ClassifyUpdateType, UpdateErrorType> m_updateErrorTypeMap;

    UpdateJobErrorMessage m_systemUpdateJobError;
    UpdateJobErrorMessage m_safeUpdateJobError;
    UpdateJobErrorMessage m_UnkonwUpdateJobError;

    TestingChannelStatus m_testingChannelStatus;
    MirrorInfoList m_mirrorList;
    bool m_smartMirrorSwitch;
};

#endif // UPDATEMODEL_H
