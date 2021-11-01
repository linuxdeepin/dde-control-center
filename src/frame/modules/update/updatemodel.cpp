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

#include "updatemodel.h"
#include "modules/systeminfo/systeminfomodel.h"

namespace dcc {
namespace update {

DownloadInfo::DownloadInfo(const qlonglong &downloadSize, const QList<AppUpdateInfo> &appInfos, QObject *parent)
    : QObject(parent)
    , m_downloadSize(downloadSize)
    , m_downloadProgress(0)
    , m_appInfos(appInfos)
{

}

void DownloadInfo::setDownloadProgress(double downloadProgress)
{
    if (compareDouble(m_downloadProgress, downloadProgress)) {
        m_downloadProgress = downloadProgress;
        Q_EMIT downloadProgressChanged(downloadProgress);
    }
}


UpdateModel::UpdateModel(QObject *parent)
    : QObject(parent)
    , m_status(UpdatesStatus::Default)
    , m_systemUpdateStatus(UpdatesStatus::Default)
    , m_appUpdateStatus(UpdatesStatus::Default)
    , m_safeUpdateStatus(UpdatesStatus::Default)
    , m_unkonowUpdateStatus(UpdatesStatus::Default)
    , m_downloadInfo(nullptr)
    , m_systemUpdateInfo(nullptr)
    , m_appUpdateInfo(nullptr)
    , m_safeUpdateInfo(nullptr)
    , m_unknownUpdateInfo(nullptr)
    , m_updateProgress(0.0)
    , m_upgradeProgress(0.0)
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    , m_sourceCheck(false)
#endif
    , m_lowBattery(false)
    , m_netselectExist(false)
    , m_autoCleanCache(false)
    , m_autoDownloadUpdates(false)
    , m_autoCheckUpdates(false)
    , m_updateNotify(false)
    , m_smartMirrorSwitch(false)
    , m_mirrorId("")
    , m_bRecoverBackingUp(false)
    , m_bRecoverConfigValid(false)
    , m_bRecoverRestoring(false)
    , m_systemVersionInfo("")
    , m_metaEnum(QMetaEnum::fromType<ModelUpdatesStatus>())
    , m_bSystemActivation(UiActiveState::Unknown)
    , m_autoCheckUpdateCircle(0)
    , m_isUpdatablePackages(false)

{

}

void UpdateModel::setMirrorInfos(const MirrorInfoList &list)
{
    m_mirrorList = list;
}

void UpdateModel::setDefaultMirror(const QString &mirrorId)
{
    if (mirrorId == "")
        return;
    m_mirrorId = mirrorId;

    QList<MirrorInfo>::iterator it = m_mirrorList.begin();
    for (; it != m_mirrorList.end(); ++it) {
        if ((*it).m_id == mirrorId) {
            Q_EMIT defaultMirrorChanged(*it);
        }
    }
}

DownloadInfo *UpdateModel::downloadInfo() const
{
    return m_downloadInfo;
}

UpdateItemInfo *UpdateModel::systemDownloadInfo() const
{
    return m_systemUpdateInfo;
}

UpdateItemInfo *UpdateModel::appDownloadInfo() const
{
    return m_appUpdateInfo;
}

UpdateItemInfo *UpdateModel::safeDownloadInfo() const
{
    return m_safeUpdateInfo;
}

UpdateItemInfo *UpdateModel::unknownDownloadInfo() const
{
    return m_unknownUpdateInfo;
}

QMap<ClassifyUpdateType, UpdateItemInfo *>  UpdateModel::allDownloadInfo() const
{
    return m_allUpdateInfos;
}

void UpdateModel::setDownloadInfo(DownloadInfo *downloadInfo)
{
    if (m_downloadInfo) {
        m_downloadInfo->deleteLater();
        m_downloadInfo = nullptr;
    }

    m_downloadInfo = downloadInfo;
    Q_EMIT downloadInfoChanged(downloadInfo);
}

void UpdateModel::setSystemDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    deleteUpdateInfo(m_systemUpdateInfo);

    m_systemUpdateInfo = updateItemInfo;
    connect(m_systemUpdateInfo, &UpdateItemInfo::downloadProgressChanged, this, &UpdateModel::systemUpdateProgressChanged);

    Q_EMIT systemUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setAppDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    deleteUpdateInfo(m_appUpdateInfo);

    m_appUpdateInfo = updateItemInfo;
    connect(m_appUpdateInfo, &UpdateItemInfo::downloadProgressChanged, this, &UpdateModel::appUpdateProgressChanged);

    Q_EMIT appUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setSafeDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    deleteUpdateInfo(m_safeUpdateInfo);
    m_safeUpdateInfo = updateItemInfo;
    connect(m_safeUpdateInfo, &UpdateItemInfo::downloadProgressChanged, this, &UpdateModel::safeUpdateProgressChanged);

    Q_EMIT safeUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setUnknownDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    deleteUpdateInfo(m_unknownUpdateInfo);
    m_unknownUpdateInfo = updateItemInfo;
    connect(m_unknownUpdateInfo, &UpdateItemInfo::downloadProgressChanged, this, &UpdateModel::unkonowUpdateProgressChanged);

    Q_EMIT unknownUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setAllDownloadInfo(QMap<ClassifyUpdateType, UpdateItemInfo *> &allUpdateInfoInfo)
{
    m_allUpdateInfos = allUpdateInfoInfo;

    setSystemDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::SystemUpdate));
    setAppDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::AppStoreUpdate));
    setSafeDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::SecurityUpdate));
    setUnknownDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::UnknownUpdate));
}

QMap<QString, int> UpdateModel::mirrorSpeedInfo() const
{
    return m_mirrorSpeedInfo;
}

void UpdateModel::setMirrorSpeedInfo(const QMap<QString, int> &mirrorSpeedInfo)
{
    m_mirrorSpeedInfo = mirrorSpeedInfo;

    if (mirrorSpeedInfo.keys().length())
        Q_EMIT mirrorSpeedInfoAvaiable(mirrorSpeedInfo);
}

bool UpdateModel::lowBattery() const
{
    return m_lowBattery;
}

void UpdateModel::setLowBattery(bool lowBattery)
{
    if (lowBattery != m_lowBattery) {
        m_lowBattery = lowBattery;
        Q_EMIT lowBatteryChanged(lowBattery);
    }
}

bool UpdateModel::autoDownloadUpdates() const
{
    return m_autoDownloadUpdates;
}

void UpdateModel::setAutoDownloadUpdates(bool autoDownloadUpdates)
{
    if (m_autoDownloadUpdates != autoDownloadUpdates) {
        m_autoDownloadUpdates = autoDownloadUpdates;
        Q_EMIT autoDownloadUpdatesChanged(autoDownloadUpdates);
    }
}

MirrorInfo UpdateModel::defaultMirror() const
{
    QList<MirrorInfo>::const_iterator it = m_mirrorList.begin();
    for (; it != m_mirrorList.end(); ++it) {
        if ((*it).m_id == m_mirrorId) {
            return *it;
        }
    }

    return m_mirrorList.at(0);
}

UpdatesStatus UpdateModel::status() const
{
    return m_status;
}

void UpdateModel::setStatus(const UpdatesStatus &status)
{
    if (getIsRecoveryBackingup(status))
        return;

    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged(status);
    }
}

void UpdateModel::setStatus(const UpdatesStatus &status, int line)
{
    qDebug() << " from work set status : " << m_metaEnum.valueToKey(status) << " , set place in work line : " << line;
    setStatus(status);
}

double UpdateModel::upgradeProgress() const
{
    return m_upgradeProgress;
}

void UpdateModel::setUpgradeProgress(double upgradeProgress)
{
    if (compareDouble(m_upgradeProgress, upgradeProgress)) {
        m_upgradeProgress = upgradeProgress;
        Q_EMIT upgradeProgressChanged(upgradeProgress);
    }
}

bool UpdateModel::autoCleanCache() const
{
    return m_autoCleanCache;
}

void UpdateModel::setAutoCleanCache(bool autoCleanCache)
{
    if (m_autoCleanCache == autoCleanCache)
        return;

    m_autoCleanCache = autoCleanCache;
    Q_EMIT autoCleanCacheChanged(autoCleanCache);
}

double UpdateModel::updateProgress() const
{
    return m_updateProgress;
}

void UpdateModel::setUpdateProgress(double updateProgress)
{
    if (compareDouble(m_updateProgress, updateProgress)) {
        m_updateProgress = updateProgress;
        Q_EMIT updateProgressChanged(updateProgress);
    }
}

bool UpdateModel::netselectExist() const
{
    return m_netselectExist;
}

void UpdateModel::setNetselectExist(bool netselectExist)
{
    if (m_netselectExist == netselectExist) {
        return;
    }

    m_netselectExist = netselectExist;

    Q_EMIT netselectExistChanged(netselectExist);
}


void UpdateModel::setAutoCheckUpdates(bool autoCheckUpdates)
{
    if (autoCheckUpdates == m_autoCheckUpdates) return;

    m_autoCheckUpdates = autoCheckUpdates;

    Q_EMIT autoCheckUpdatesChanged(autoCheckUpdates);
}

void UpdateModel::setUpdateMode(quint64 updateMode)
{
    qDebug() << Q_FUNC_INFO << "get UpdateMode from dbus:" << updateMode;

    if (m_updateMode == updateMode) {
        return;
    }

    m_updateMode = updateMode;

    setAutoCheckSystemUpdates(m_updateMode & 0b0001);
    setAutoCheckAppUpdates((m_updateMode & 0b0010) >> 1);
    setAutoCheckSecureUpdates((m_updateMode & 0b0100) >> 2);
}

void UpdateModel::setAutoCheckSecureUpdates(bool autoCheckSecureUpdates)
{
    if (autoCheckSecureUpdates == m_autoCheckSecureUpdates)
        return;

    m_autoCheckSecureUpdates = autoCheckSecureUpdates;

    Q_EMIT autoCheckSecureUpdatesChanged(autoCheckSecureUpdates);
}

void UpdateModel::setAutoCheckSystemUpdates(bool autoCheckSystemUpdates)
{
    if (autoCheckSystemUpdates == m_autoCheckSystemUpdates)
        return;

    m_autoCheckSystemUpdates = autoCheckSystemUpdates;

    Q_EMIT autoCheckSystemUpdatesChanged(autoCheckSystemUpdates);
}

void UpdateModel::setAutoCheckAppUpdates(bool autoCheckAppUpdates)
{
    if (autoCheckAppUpdates == m_autoCheckAppUpdates)
        return;

    m_autoCheckAppUpdates = autoCheckAppUpdates;

    Q_EMIT autoCheckAppUpdatesChanged(autoCheckAppUpdates);
}

void UpdateModel::setSmartMirrorSwitch(bool smartMirrorSwitch)
{
    if (m_smartMirrorSwitch == smartMirrorSwitch) return;

    m_smartMirrorSwitch = smartMirrorSwitch;

    Q_EMIT smartMirrorSwitchChanged(smartMirrorSwitch);
}

void UpdateModel::setRecoverBackingUp(bool recoverBackingUp)
{
    if (m_bRecoverBackingUp == recoverBackingUp)
        return;

    m_bRecoverBackingUp = recoverBackingUp;

    Q_EMIT recoverBackingUpChanged(recoverBackingUp);
}

void UpdateModel::setRecoverConfigValid(bool recoverConfigValid)
{
    if (m_bRecoverConfigValid == recoverConfigValid)
        return;

    m_bRecoverConfigValid = recoverConfigValid;

    Q_EMIT recoverConfigValidChanged(recoverConfigValid);
}

void UpdateModel::setRecoverRestoring(bool recoverRestoring)
{
    if (m_bRecoverRestoring == recoverRestoring)
        return;

    m_bRecoverRestoring = recoverRestoring;

    Q_EMIT recoverRestoringChanged(recoverRestoring);
}

void UpdateModel::setSystemVersionInfo(const QString &systemVersionInfo)
{
    if (m_systemVersionInfo == systemVersionInfo)
        return;

    m_systemVersionInfo = systemVersionInfo;

    Q_EMIT systemVersionChanged(systemVersionInfo);
}

void UpdateModel::setSystemActivation(const UiActiveState &systemactivation)
{
    if (m_bSystemActivation == systemactivation) {
        return;
    }
    m_bSystemActivation = systemactivation;

    Q_EMIT systemActivationChanged(systemactivation);
}

void UpdateModel::isUpdatablePackages(bool isUpdatablePackages)
{
    if (m_isUpdatablePackages == isUpdatablePackages)
        return;

    m_isUpdatablePackages = isUpdatablePackages;
    Q_EMIT updatablePackagesChanged(isUpdatablePackages);
}

//判断当前是否正在备份中，若正在备份则不能再设置其他状态，直到备份有结果了才能继续设置其他状态
bool UpdateModel::getIsRecoveryBackingup(UpdatesStatus state) const
{
    bool ret = true;

    if (m_status == UpdatesStatus::RecoveryBackingup) {
        if (state == UpdatesStatus::RecoveryBackingSuccessed ||
                state == UpdatesStatus::RecoveryBackupFailed) {
            ret = false;
            qDebug() << " Backing up End ! , state : " << state;
        } else {
            qDebug() << " Now is Backing up , can't set other status. Please wait..." << m_metaEnum.valueToKey(state);
        }
    } else {
        ret = false;
    }

    return ret;
}

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
bool UpdateModel::sourceCheck() const
{
    return m_sourceCheck;
}

void UpdateModel::setSourceCheck(bool sourceCheck)
{
    if (m_sourceCheck == sourceCheck)
        return;

    m_sourceCheck = sourceCheck;

    Q_EMIT sourceCheckChanged(sourceCheck);
}
#endif

void UpdateModel::setLastCheckUpdateTime(const QString &lastTime)
{
    qDebug() << "Last check time:" << lastTime;
    m_lastCheckUpdateTime = lastTime.left(QString("0000-00-00 00:00:00").size());
}

void UpdateModel::setHistoryAppInfos(const QList<AppUpdateInfo> &infos)
{
    m_historyAppInfos = infos;
}

void UpdateModel::setAutoCheckUpdateCircle(const int interval)
{
    m_autoCheckUpdateCircle = interval;
}

bool UpdateModel::enterCheckUpdate()
{
    qDebug() << "last update time:" << m_lastCheckUpdateTime << "check circle:" << m_autoCheckUpdateCircle;
    return QDateTime::fromString(m_lastCheckUpdateTime, "yyyy-MM-dd hh:mm:ss").secsTo(QDateTime::currentDateTime()) > m_autoCheckUpdateCircle * 3600;
}

void UpdateModel::setUpdateNotify(const bool notify)
{
    if (m_updateNotify == notify) {
        return;
    }

    m_updateNotify = notify;

    Q_EMIT updateNotifyChanged(notify);
}

UpdatesStatus UpdateModel::getUnkonowUpdateStatus() const
{
    return m_unkonowUpdateStatus;
}

void UpdateModel::setUnkonowUpdateStatus(const UpdatesStatus &unkonowUpdateStatus)
{
    if (m_unkonowUpdateStatus != unkonowUpdateStatus) {
        m_unkonowUpdateStatus = unkonowUpdateStatus;
        Q_EMIT unkonowUpdateStatusChanged(unkonowUpdateStatus);
    }
}

UpdatesStatus UpdateModel::getSafeUpdateStatus() const
{
    return m_safeUpdateStatus;
}

void UpdateModel::setSafeUpdateStatus(const UpdatesStatus &safeUpdateStatus)
{
    if (m_safeUpdateStatus != safeUpdateStatus) {
        m_safeUpdateStatus = safeUpdateStatus;
        Q_EMIT safeUpdateStatusChanged(safeUpdateStatus);
    }
}

UpdatesStatus UpdateModel::getAppUpdateStatus() const
{
    return m_appUpdateStatus;
}

void UpdateModel::setAppUpdateStatus(const UpdatesStatus &appUpdateStatus)
{

    if (m_appUpdateStatus != appUpdateStatus) {
        m_appUpdateStatus = appUpdateStatus;
        Q_EMIT appUpdateStatusChanged(appUpdateStatus);
    }
}

UpdatesStatus UpdateModel::getSystemUpdateStatus() const
{
    return m_systemUpdateStatus;
}

void UpdateModel::setSystemUpdateStatus(const UpdatesStatus &systemUpdateStatus)
{
    if (m_systemUpdateStatus != systemUpdateStatus) {
        m_systemUpdateStatus = systemUpdateStatus;
        Q_EMIT systemUpdateStatusChanged(systemUpdateStatus);
    }

}

UpdateModel::~UpdateModel()
{
    if (m_systemUpdateInfo != nullptr) {
        delete m_systemUpdateInfo;
        m_systemUpdateInfo = nullptr;
    }

    if (m_appUpdateInfo != nullptr) {
        delete m_appUpdateInfo;
        m_appUpdateInfo = nullptr;
    }

    if (m_safeUpdateInfo != nullptr) {
        delete m_safeUpdateInfo;
        m_safeUpdateInfo = nullptr;
    }

    if (m_unknownUpdateInfo != nullptr) {
        delete m_unknownUpdateInfo;
        m_unknownUpdateInfo = nullptr;
    }
}

void UpdateModel::setClassifyUpdateTypeStatus(ClassifyUpdateType type, UpdatesStatus status)
{
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        setSystemUpdateStatus(status);
        break;
    case ClassifyUpdateType::AppStoreUpdate:
        setAppUpdateStatus(status);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        setSafeUpdateStatus(status);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        setUnkonowUpdateStatus(status);
        break;
    default:
        break;
    }
}

void UpdateModel::setAllClassifyUpdateStatus(UpdatesStatus status)
{
    setSystemUpdateStatus(status);
    setAppUpdateStatus(status);
    setSafeUpdateStatus(status);
    setUnkonowUpdateStatus(status);
}

void UpdateModel::deleteUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    if (!updateItemInfo) {
        updateItemInfo->deleteLater();
        updateItemInfo = nullptr;
    }
}

bool UpdateModel::getAutoInstallUpdates() const
{
    return m_autoInstallUpdates;
}

void UpdateModel::setAutoInstallUpdates(bool autoInstallUpdates)
{
    if (m_autoInstallUpdates != autoInstallUpdates) {
        m_autoInstallUpdates = autoInstallUpdates;
        Q_EMIT autoInstallUpdatesChanged(autoInstallUpdates);
    }
}

quint64 UpdateModel::getAutoInstallUpdateType() const
{
    return m_autoInstallUpdateType;
}

void UpdateModel::setAutoInstallUpdateType(const quint64 &autoInstallUpdateType)
{
    if (m_autoInstallUpdateType != autoInstallUpdateType) {
        m_autoInstallUpdateType = autoInstallUpdateType;
        Q_EMIT autoInstallUpdateTypeChanged(autoInstallUpdateType);
    }
}

UpdatesStatus UpdateModel::getClassifyUpdateStatus(ClassifyUpdateType type)
{
    UpdatesStatus status = UpdatesStatus::Default;
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        status = getSystemUpdateStatus();
        break;
    case ClassifyUpdateType::AppStoreUpdate:
        status = getAppUpdateStatus();
        break;
    case ClassifyUpdateType::SecurityUpdate:
        status = getSafeUpdateStatus();
        break;
    case ClassifyUpdateType::UnknownUpdate:
        status = getUnkonowUpdateStatus();
        break;
    default:
        break;

    }
    return status;
}

}
}
