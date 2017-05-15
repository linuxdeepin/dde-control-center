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

signals:
    void autoDownloadUpdatesChanged(const bool &autoDownloadUpdates);
    void defaultMirrorChanged(const MirrorInfo &mirror);

    void lowBatteryChanged(const bool &lowBattery);
    void statusChanged(const UpdatesStatus &status);

    void mirrorSpeedInfoAvaiable(const QMap<QString, int> &mirrorSpeedInfo);

    void downloadInfoChanged(DownloadInfo *downloadInfo);

    void upgradeProgressChanged(const double &upgradeProgress);

private:
    UpdatesStatus m_status;
    DownloadInfo *m_downloadInfo;

    double m_upgradeProgress;

    bool m_lowBattery;

    bool m_autoDownloadUpdates;
    QString m_mirrorId;
    MirrorInfoList m_mirrorList;
    QMap<QString, int> m_mirrorSpeedInfo;
};

}
}
#endif // UPDATEMODEL_H
