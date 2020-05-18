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

namespace dcc{
namespace update{

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
    , m_status(UpdatesStatus::Checking)
    , m_downloadInfo(nullptr)
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
    , m_smartMirrorSwitch(false)
    , m_mirrorId("")
    , m_bRecoverBackingUp(false)
    , m_bRecoverConfigValid(false)
    , m_bRecoverRestoring(false)
    , m_systemVersionInfo("")
    , m_bSystemActivation(false)
{
    m_metaEnum = QMetaEnum::fromType<ModelUpdatesStatus>();
}

void UpdateModel::setMirrorInfos(const MirrorInfoList &list)
{
    m_mirrorList = list;
}

void UpdateModel::setDefaultMirror(const QString &mirror)
{
    if (mirror == "")
        return;
    m_mirrorId = mirror;

    QList<MirrorInfo>::iterator it = m_mirrorList.begin();
    for(; it != m_mirrorList.end(); ++it)
    {
        if((*it).m_id == mirror)
        {
            Q_EMIT defaultMirrorChanged(*it);
        }
    }
}

DownloadInfo *UpdateModel::downloadInfo() const
{
    return m_downloadInfo;
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
    for(; it != m_mirrorList.end(); ++it)
    {
        if((*it).m_id == m_mirrorId)
        {
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

void UpdateModel::setSystemVersionInfo(QString systemVersionInfo)
{
    if (m_systemVersionInfo == systemVersionInfo)
        return;

    m_systemVersionInfo = systemVersionInfo;

    Q_EMIT systemVersionChanged(systemVersionInfo);
}

void UpdateModel::setSystemActivation(quint32 systemactivation)
{
    if (systemactivation == systeminfo::ActiveState::Authorized || systemactivation == systeminfo::ActiveState::TrialAuthorized) {
        m_bSystemActivation = true;
    } else {
        m_bSystemActivation = false;
    }

    Q_EMIT setSystemActivationChanged(systemactivation);
}

void UpdateModel::isUpdatablePackages(bool isUpdatablePackages)
{
    if (m_isUpdatablePackages == isUpdatablePackages)
        return;

    m_isUpdatablePackages = isUpdatablePackages;
}

void UpdateModel::isAutoCheckUpdates(bool isAutoCheckUpdates)
{
    if (m_isAutoCheckUpdates == isAutoCheckUpdates)
        return;

    m_isAutoCheckUpdates = isAutoCheckUpdates;
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

}
}
