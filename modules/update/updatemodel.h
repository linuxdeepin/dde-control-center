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

#ifndef UPDATEMODEL_H
#define UPDATEMODEL_H

#include <QObject>
#include <com_deepin_lastore_updater.h>

#include "common.h"

namespace dcc{
namespace update{

class DownloadInfo : public QObject
{
    Q_OBJECT
public:
    explicit DownloadInfo(const qlonglong &downloadSize, const QList<AppUpdateInfo> &appInfos, QObject *parent = 0);
    virtual ~DownloadInfo() {}

    inline qlonglong downloadSize() const { return m_downloadSize; }
    double downloadProgress() const { return m_downloadProgress; }
    QList<AppUpdateInfo> appInfos() const { return m_appInfos; }

    void setDownloadProgress(double downloadProgress);

signals:
    void downloadProgressChanged(const double &progress);

private:
    qlonglong m_downloadSize;
    double m_downloadProgress;
    QList<AppUpdateInfo> m_appInfos;
};

class UpdateModel : public QObject
{
    Q_OBJECT
public:
    explicit UpdateModel(QObject *parent = 0);

    void setMirrorInfos(const MirrorInfoList& list);
    MirrorInfoList mirrorInfos() const { return m_mirrorList;}

    UpdatesStatus status() const;
    void setStatus(const UpdatesStatus &status);

    MirrorInfo defaultMirror() const;
    void setDefaultMirror(const QString& mirrorId);

    DownloadInfo *downloadInfo() const;
    void setDownloadInfo(DownloadInfo *downloadInfo);

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

    bool sourceCheck() const;
    void setSourceCheck(bool sourceCheck);

signals:
    void autoDownloadUpdatesChanged(const bool &autoDownloadUpdates);
    void defaultMirrorChanged(const MirrorInfo &mirror);

    void lowBatteryChanged(const bool &lowBattery);
    void statusChanged(const UpdatesStatus &status);

    void sourceCheckChanged(bool sourceCheck);

    void mirrorSpeedInfoAvaiable(const QMap<QString, int> &mirrorSpeedInfo);

    void downloadInfoChanged(DownloadInfo *downloadInfo);
    void updateProgressChanged(const double &updateProgress);
    void upgradeProgressChanged(const double &upgradeProgress);
    void autoCleanCacheChanged(const bool autoCleanCache);

private:
    UpdatesStatus m_status;
    DownloadInfo *m_downloadInfo;

    double m_updateProgress;
    double m_upgradeProgress;

    bool m_sourceCheck;

    bool m_lowBattery;

    bool m_autoCleanCache;
    bool m_autoDownloadUpdates;
    QString m_mirrorId;
    MirrorInfoList m_mirrorList;
    QMap<QString, int> m_mirrorSpeedInfo;
};

}
}
#endif // UPDATEMODEL_H
