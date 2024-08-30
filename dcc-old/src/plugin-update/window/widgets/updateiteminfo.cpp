//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "updateiteminfo.h"

UpdateItemInfo::UpdateItemInfo(QObject *parent)
    : QObject(parent)
    , m_downloadSize(0)
    , m_downloadProgress(0)
    , m_installProgress(0)
    , m_packageId("")
    , m_name("")
    , m_currentVersion("")
    , m_availableVersion("")
    , m_explain("")
    , m_updateTime("")
{

}

void UpdateItemInfo::setDownloadProgress(double downloadProgress)
{
    if (downloadProgress != m_downloadProgress) {
        m_downloadProgress = downloadProgress;
        Q_EMIT downloadProgressChanged(downloadProgress);
    }
}

void UpdateItemInfo::setDownloadSize(qlonglong downloadSize)
{
    if (downloadSize != m_downloadSize) {
        m_downloadSize = downloadSize;
        Q_EMIT downloadSizeChanged(downloadSize);
    }
}

void UpdateItemInfo::setInstallProgress(double installProgress)
{
    if (installProgress != m_installProgress) {
        m_installProgress = installProgress;
        Q_EMIT installProgressChanged(installProgress);
    }
}

QString UpdateItemInfo::packageId() const
{
    return m_packageId;
}

void UpdateItemInfo::setPackageId(const QString &packageId)
{
    m_packageId = packageId;
}

QString UpdateItemInfo::name() const
{
    return m_name;
}

void UpdateItemInfo::setName(const QString &name)
{
    m_name = name;
}

QString UpdateItemInfo::currentVersion() const
{
    return m_currentVersion;
}

void UpdateItemInfo::setCurrentVersion(const QString &currentVersion)
{
    m_currentVersion = currentVersion;
}

QString UpdateItemInfo::availableVersion() const
{
    return m_availableVersion;
}

void UpdateItemInfo::setAvailableVersion(const QString &availableVersion)
{
    m_availableVersion = availableVersion;
}

QString UpdateItemInfo::explain() const
{
    return m_explain;
}

void UpdateItemInfo::setExplain(const QString &explain)
{
    m_explain = explain;
}


QString UpdateItemInfo::updateTime() const
{
    return m_updateTime;
}

void UpdateItemInfo::setUpdateTime(const QString &updateTime)
{
    m_updateTime = updateTime;
}

QList<DetailInfo> UpdateItemInfo::detailInfos() const
{
    return m_detailInfos;
}

void UpdateItemInfo::setDetailInfos(QList<DetailInfo> &detailInfos)
{
    m_detailInfos.clear();
    m_detailInfos = detailInfos;
}

void UpdateItemInfo::addDetailInfo(DetailInfo detailInfo)
{
    m_detailInfos.append(std::move(detailInfo));
}
