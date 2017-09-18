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

#include "updatemodel.h"

namespace dcc{
namespace update{

DownloadInfo::DownloadInfo(const qlonglong &downloadSize, const QList<AppUpdateInfo> &appInfos, QObject *parent) :
    QObject(parent),
    m_downloadSize(downloadSize),
    m_downloadProgress(0),
    m_appInfos(appInfos)
{

}

void DownloadInfo::setDownloadProgress(double downloadProgress)
{
    if (m_downloadProgress != downloadProgress) {
        m_downloadProgress = downloadProgress;
        emit downloadProgressChanged(downloadProgress);
    }
}


UpdateModel::UpdateModel(QObject *parent) :
    QObject(parent),
    m_status(UpdatesStatus::Checking),
    m_downloadInfo(nullptr),
    m_upgradeProgress(0),
    m_lowBattery(false),
    m_autoDownloadUpdates(true),
    m_mirrorId("")
{

}

void UpdateModel::setMirrorInfos(const MirrorInfoList &list)
{
    m_mirrorList = list;
}

void UpdateModel::setDefaultMirror(const QString &mirror)
{
    if (mirror == m_mirrorId) return;
    m_mirrorId = mirror;

    QList<MirrorInfo>::iterator it = m_mirrorList.begin();
    for(; it != m_mirrorList.end(); ++it)
    {
        if((*it).m_id == mirror)
        {
            emit defaultMirrorChanged(*it);
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
    emit downloadInfoChanged(downloadInfo);
}

QMap<QString, int> UpdateModel::mirrorSpeedInfo() const
{
    return m_mirrorSpeedInfo;
}

void UpdateModel::setMirrorSpeedInfo(const QMap<QString, int> &mirrorSpeedInfo)
{
    m_mirrorSpeedInfo = mirrorSpeedInfo;

    if (mirrorSpeedInfo.keys().length())
        emit mirrorSpeedInfoAvaiable(mirrorSpeedInfo);
}

bool UpdateModel::lowBattery() const
{
    return m_lowBattery;
}

void UpdateModel::setLowBattery(bool lowBattery)
{
    if (lowBattery != m_lowBattery) {
        m_lowBattery = lowBattery;
        emit lowBatteryChanged(lowBattery);
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
        emit autoDownloadUpdatesChanged(autoDownloadUpdates);
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
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

double UpdateModel::upgradeProgress() const
{
    return m_upgradeProgress;
}

void UpdateModel::setUpgradeProgress(double upgradeProgress)
{
    if (m_upgradeProgress != upgradeProgress) {
        m_upgradeProgress = upgradeProgress;
        emit upgradeProgressChanged(upgradeProgress);
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
    emit autoCleanCacheChanged(autoCleanCache);
}

}
}
