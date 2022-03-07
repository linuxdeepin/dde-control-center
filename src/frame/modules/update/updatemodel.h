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

#ifndef UPDATEMODEL_H
#define UPDATEMODEL_H

#include <QObject>
#include <com_deepin_lastore_updater.h>

#include "common.h"
#include "widgets/utils.h"
#include "updateiteminfo.h"

namespace dcc {
namespace update {

class DownloadInfo : public QObject
{
    Q_OBJECT
public:
    explicit DownloadInfo(const qlonglong &downloadSize, const QList<AppUpdateInfo> &appInfos, QObject *parent = 0);
    virtual ~DownloadInfo() {}

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

struct UpdateJobErrorMessage {
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
    //ModelUpdatesStatus仅用于log显示；
    //common.h UpdatesStatu更新后，此处需要同步更新
    enum ModelUpdatesStatus {
        Default,
        Checking,
        Updated,
        UpdatesAvailable,
        Downloading,
        DownloadPaused,
        Downloaded,
        Installing,
        UpdateSucceeded,
        UpdateFailed,
        NeedRestart,
        NoNetwork,
        NoSpace,
        DeependenciesBrokenError,
        RecoveryBackingup,
        RecoveryBackingSuccessed,
        RecoveryBackupFailed
    };
    Q_ENUM(ModelUpdatesStatus)

    void setMirrorInfos(const MirrorInfoList &list);
    MirrorInfoList mirrorInfos() const { return m_mirrorList;}

    UpdatesStatus status() const;
    void setStatus(const UpdatesStatus &status);
    void setStatus(const UpdatesStatus &status, int line);

    MirrorInfo defaultMirror() const;
    void setDefaultMirror(const QString &mirrorId);

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

    QMap<QString, int> mirrorSpeedInfo() const;
    void setMirrorSpeedInfo(const QMap<QString, int> &mirrorSpeedInfo);

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

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    bool sourceCheck() const;
    void setSourceCheck(bool sourceCheck);
#endif

    bool netselectExist() const;
    void setNetselectExist(bool netselectExist);

    inline bool autoCheckUpdates() const { return m_autoCheckUpdates; }
    void setAutoCheckUpdates(bool autoCheckUpdates);

    inline quint64 updateMode() const { return m_updateMode; }
    void setUpdateMode(quint64 updateMode);

    inline bool autoCheckSecureUpdates() const { return m_autoCheckSecureUpdates; }
    void setAutoCheckSecureUpdates(bool autoCheckSecureUpdates);

    inline bool autoCheckSystemUpdates() const { return m_autoCheckSystemUpdates; }
    void setAutoCheckSystemUpdates(bool autoCheckSystemUpdates);

    inline bool autoCheckAppUpdates() const { return m_autoCheckAppUpdates; }
    void setAutoCheckAppUpdates(bool autoCheckAppUpdates);

    bool smartMirrorSwitch() const { return m_smartMirrorSwitch; }
    void setSmartMirrorSwitch(bool smartMirrorSwitch);

    inline bool recoverBackingUp() const { return m_bRecoverBackingUp; }
    void setRecoverBackingUp(bool recoverBackingUp);

    inline bool recoverConfigValid() const { return m_bRecoverConfigValid; }
    void setRecoverConfigValid(bool recoverConfigValid);

    inline bool recoverRestoring() const { return m_bRecoverRestoring; }
    void setRecoverRestoring(bool recoverRestoring);

    inline QString systemVersionInfo() const { return m_systemVersionInfo; }
    void setSystemVersionInfo(const QString &systemVersionInfo);

    bool getIsRecoveryBackingup(UpdatesStatus state) const;

    inline UiActiveState systemActivation() const {return m_bSystemActivation;}
    void setSystemActivation(const UiActiveState &systemactivation);

    inline bool getUpdatablePackages() const {return m_isUpdatablePackages;}
    void isUpdatablePackages(bool isUpdatablePackages);

    const QString &lastCheckUpdateTime() const {return m_lastCheckUpdateTime;}
    void setLastCheckUpdateTime(const QString &lastTime);

    const QList<AppUpdateInfo> &historyAppInfos() const {return m_historyAppInfos;}
    void setHistoryAppInfos(const QList<AppUpdateInfo> &infos);

    int autoCheckUpdateCircle() const {return m_autoCheckUpdateCircle;}
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

    void setClassityUpdateJonError(ClassifyUpdateType type, const UpdateJobErrorMessage &UnkonwUpdateJobError);

    bool getAutoCheckThirdpartyUpdates() const;
    void setAutoCheckThirdpartyUpdates(bool autoCheckThirdpartyUpdates);

Q_SIGNALS:
    void autoDownloadUpdatesChanged(const bool &autoDownloadUpdates);
    void autoInstallUpdatesChanged(const bool &autoInstallUpdates);
    void autoInstallUpdateTypeChanged(const quint64 &autoInstallUpdateType);
    void defaultMirrorChanged(const MirrorInfo &mirror);

    void smartMirrorSwitchChanged(bool smartMirrorSwitch);

    void lowBatteryChanged(const bool &lowBattery);
    void statusChanged(const UpdatesStatus &status);

    void systemUpdateStatusChanged(const UpdatesStatus &status);
    void safeUpdateStatusChanged(const UpdatesStatus &status);
    void unkonowUpdateStatusChanged(const UpdatesStatus &status);

    void systemUpdateDownloadSizeChanged(const qlonglong updateSize);
    void safeUpdateDownloadSizeChanged(const qlonglong updateSize);
    void unkonowUpdateDownloadSizeChanged(const qlonglong updateSize);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void sourceCheckChanged(bool sourceCheck);
#endif
    void mirrorSpeedInfoAvaiable(const QMap<QString, int> &mirrorSpeedInfo);

    void downloadInfoChanged(DownloadInfo *downloadInfo);

    void systemUpdateInfoChanged(UpdateItemInfo *updateItemInfo);
    void safeUpdateInfoChanged(UpdateItemInfo *updateItemInfo);
    void unknownUpdateInfoChanged(UpdateItemInfo *updateItemInfo);

    void classityUpdateJobErrorChanged(ClassifyUpdateType type, const QString &errorMessage);

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
    void recoverBackingUpChanged(bool recoverBackingUp);
    void recoverConfigValidChanged(bool recoverConfigValid);
    void recoverRestoringChanged(bool recoverRestoring);
    void systemVersionChanged(QString version);
    void systemActivationChanged(UiActiveState systemactivation);
    void beginCheckUpdate();
    void updateCheckUpdateTime();
    void updateHistoryAppInfos();
    void updateNotifyChanged(const bool notify);
    void updatablePackagesChanged(const bool isUpdatablePackages);

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

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    bool m_sourceCheck;
#endif

    bool m_lowBattery;
    bool m_netselectExist;
    bool m_autoCleanCache;
    bool m_autoDownloadUpdates;
    bool m_autoInstallUpdates;
    quint64 m_autoInstallUpdateType;
    bool m_autoCheckUpdates;
    quint64 m_updateMode;
    bool m_autoCheckSecureUpdates;
    bool m_autoCheckSystemUpdates;
    bool m_autoCheckAppUpdates;
    bool m_autoCheckThirdpartyUpdates;
    bool m_updateNotify;
    bool m_smartMirrorSwitch;
    QString m_mirrorId;
    MirrorInfoList m_mirrorList;
    QMap<QString, int> m_mirrorSpeedInfo;
    bool m_bRecoverBackingUp;
    bool m_bRecoverConfigValid;
    bool m_bRecoverRestoring;
    QString m_systemVersionInfo;
    QMetaEnum m_metaEnum;
    UiActiveState m_bSystemActivation;

    QString m_lastCheckUpdateTime; //上次检查更新时间
    QList<AppUpdateInfo> m_historyAppInfos; //历史更新应用列表
    int m_autoCheckUpdateCircle; //决定进入检查更新界面是否自动检查,单位：小时
    bool m_isUpdatablePackages;

    UpdateJobErrorMessage m_systemUpdateJobError;
    UpdateJobErrorMessage m_safeUpdateJobError;
    UpdateJobErrorMessage m_UnkonwUpdateJobError;
};

}
}
#endif // UPDATEMODEL_H
