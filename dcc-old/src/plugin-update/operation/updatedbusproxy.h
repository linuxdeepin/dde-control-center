// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEDBUSPROXY_H
#define UPDATEDBUSPROXY_H

#include "interface/namespace.h"

#include <DDBusInterface>

#include <QDBusObjectPath>
#include <QDBusPendingReply>
#include <QObject>

typedef QMap<QString, QStringList> LastoreUpdatePackagesInfo;
typedef QMap<QString, double> BatteryPercentageInfo;

using Dtk::Core::DDBusInterface;

class QDBusMessage;
class QDBusInterface;

class UpdateDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit UpdateDBusProxy(QObject *parent = nullptr);
    ~UpdateDBusProxy();

public:
    // hostname
    QString staticHostname() const;

    // updater
    Q_PROPERTY(bool UpdateNotify READ updateNotify NOTIFY UpdateNotifyChanged)
    bool updateNotify();
    void SetUpdateNotify(bool in0);
    LastoreUpdatePackagesInfo classifiedUpdatablePackages();
    double GetCheckIntervalAndTime(QString &out1);

    Q_PROPERTY(bool AutoDownloadUpdates READ autoDownloadUpdates NOTIFY AutoDownloadUpdatesChanged)
    bool autoDownloadUpdates();
    void SetAutoDownloadUpdates(bool in0);

    Q_PROPERTY(bool AutoInstallUpdates READ autoInstallUpdates WRITE setAutoInstallUpdates NOTIFY
                       AutoInstallUpdatesChanged)
    bool autoInstallUpdates();
    void setAutoInstallUpdates(bool value);

    qulonglong autoInstallUpdateType();

    Q_PROPERTY(bool AutoCheckUpdates READ autoCheckUpdates NOTIFY AutoCheckUpdatesChanged)
    bool autoCheckUpdates();
    void SetAutoCheckUpdates(bool in0);
    Q_PROPERTY(QString MirrorSource READ mirrorSource NOTIFY MirrorSourceChanged)
    QString mirrorSource() const;
    void SetMirrorSource(const QString &in0);

    // ManagerInter
    Q_PROPERTY(bool AutoClean READ autoClean NOTIFY AutoCleanChanged)
    bool autoClean();

    Q_PROPERTY(qulonglong UpdateMode READ updateMode WRITE setUpdateMode NOTIFY UpdateModeChanged)
    uint updateMode();
    void setUpdateMode(qulonglong value);

    Q_PROPERTY(QList<QDBusObjectPath> JobList READ jobList NOTIFY JobListChanged)
    QList<QDBusObjectPath> jobList();

    QString hardwareId();

    QDBusPendingReply<QDBusObjectPath> UpdateSource();
    void CleanJob(const QString &in0);
    void SetAutoClean(bool in0);
    void StartJob(const QString &in0);
    void PauseJob(const QString &in0);
    QDBusPendingReply<QDBusObjectPath> InstallPackage(const QString &jobname,
                                                      const QString &packages);
    QDBusPendingReply<QDBusObjectPath> RemovePackage(const QString &jobname,
                                                     const QString &packages);
    QDBusPendingReply<QList<QDBusObjectPath> > ClassifiedUpgrade(qulonglong in0);
    QDBusPendingReply<qlonglong> PackagesDownloadSize(const QStringList &in0);
    QDBusPendingReply<bool> PackageExists(const QString &pkgid);

    // Power
    bool onBattery();
    BatteryPercentageInfo batteryPercentage();

    // Atomic Upgrade
    void commit(const QString &commitDate);

    bool atomBackupIsRunning();
    // Smart Mirror
    Q_PROPERTY(bool Enable READ enable NOTIFY EnableChanged)
    bool enable() const;
    void SetEnable(bool enable);
signals:
    // updater
    void UpdateNotifyChanged(bool value) const;
    void AutoDownloadUpdatesChanged(bool value) const;
    void AutoInstallUpdatesChanged(bool value) const;
    void AutoInstallUpdateTypeChanged(qulonglong value) const;
    void MirrorSourceChanged(const QString &value) const;
    void AutoCheckUpdatesChanged(bool value) const;
    void ClassifiedUpdatablePackagesChanged(LastoreUpdatePackagesInfo value) const;

    // ManagerInter
    void JobListChanged(const QList<QDBusObjectPath> &value) const;
    void AutoCleanChanged(bool value) const;
    void UpdateModeChanged(qulonglong value) const;

    // Power
    void OnBatteryChanged(bool value) const;
    void BatteryPercentageChanged(BatteryPercentageInfo value) const;

    // Atomic Upgrade
    void StateChanged(int operate, int state, QString version, QString message);
    void RunningChanged(bool value) const;
    // Smart Mirror
    void EnableChanged(bool enable);

private:
    DDBusInterface *m_hostname1Inter;
    DDBusInterface *m_updateInter;
    DDBusInterface *m_managerInter;
    DDBusInterface *m_powerInter;
    DDBusInterface *m_atomicUpgradeInter;
    DDBusInterface *m_smartMirrorInter;
};

#endif // UPDATEDBUSPROXY_H
